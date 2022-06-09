#include "../cmn/out.hpp"
#include "../cmn/target.hpp"
#include "../cmn/textTable.hpp"
#include "asmCodeGen.hpp"
#include "lir.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"

namespace liam {

void asmArgWriter::write(lirInstr& i)
{
   auto& args = i.getArgs();
   for(auto it=args.begin();it!=args.end();++it)
      write(i.orderNum,**it);
}

void asmArgWriter::write(size_t orderNum, lirArg& a)
{
   if(!m_first)
      m_w[1] << ",";
   m_w[1] << " ";

   size_t stor = m_v.demand(a).getStorageFor(orderNum,a);
   size_t framePtr = cmn::tgt::kStorageStackFramePtr;

   if(cmn::tgt::isVStack(stor))
   {
      stor = m_v.getVirtualStack().mapToReal(stor);
      framePtr = cmn::tgt::kStorageStackPtr;
   }

   if(cmn::tgt::isStackStorage(stor))
   {
      m_w[1] << "[" << m_t.getProc().getRegName(framePtr);
      writeDispIf(cmn::tgt::getStackDisp(stor) + a.disp);
      m_w[1] << "]";
   }
   else if(a.addrOf)
   {
      m_w[1] << "[" << m_t.getProc().getRegName(stor);
      writeDispIf(a.disp);
      m_w[1] << "]";
   }
   else
   {
      if(stor == cmn::tgt::kStorageImmediate)
      {
         if(auto *pLbl = dynamic_cast<lirArgLabel*>(&a))
         {
            if(pLbl->isCode)
               m_w[1] << a.getName();
            else
               m_w[1] << "qwordptr " << a.getName();
         }
         else
            m_w[1] << a.getName();
      }
      else
         m_w[1] << m_t.getProc().getRegName(stor);
      writeDispIf(a.disp);
   }

   m_first = false;
}

void asmArgWriter::writeDispIf(const __int64& disp)
{
   if(disp)
   {
      if(disp > 0)
         m_w[1] << "+";
      m_w[1] << disp;
   }
}

void asmCodeGen::generate(lirStream& s, varTable& v, varFinder& f, cmn::tgt::iTargetInfo& t, cmn::outStream& o)
{
   cmn::textTable tt;
   cmn::textTableLineWriter w(tt);
   asmCodeGen self(v,f,t,w);

   lirInstr *pInstr = &s.pTail->head();
   while(true)
   {
      self.handleInstr(*pInstr);

      if(pInstr->isLast())
         break;
      else
         pInstr = &pInstr->next();
   }

   tt.compileAndWrite(o.stream());
   o.stream() << std::endl << std::endl;
}

void asmCodeGen::handleInstr(lirInstr& i)
{
   switch(i.instrId)
   {
      case cmn::tgt::kSelectSegment:
         {
            m_w[0] << ".seg " << i.getArgs()[0]->getName();
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kEnterFunc:
         {
            m_w[0] << i.comment << ":";
            m_w.advanceLine();
            m_w[1] << "push, " << m_t.getProc().getRegName(cmn::tgt::kStorageStackFramePtr);
            m_w.advanceLine();
            auto& regs = m_f.getUsedRegs();
            for(auto it=regs.begin();it!=regs.end();++it)
            {
               if(m_t.getCallConvention().requiresPrologEpilogSave(*it))
               {
                  m_w[1] << "push, " << m_t.getProc().getRegName(*it);
                  m_w.advanceLine();
               }
            }
            m_w[1] << "mov, "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackFramePtr) << ", "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr);
            m_w.advanceLine();
            size_t locals = m_f.getUsedStackSpace();
            if(locals)
            {
               m_w[1]
                  << "sub, "
                  << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr) << ", "
                  << locals;
               m_w.advanceLine();
            }
         }
         break;
      case cmn::tgt::kExitFunc:
         {
            m_w[1] << "mov, "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr) << ", "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackFramePtr);
            m_w.advanceLine();
            auto& regs = m_f.getUsedRegs();
            for(auto it=regs.rbegin();it!=regs.rend();++it)
            {
               if(m_t.getCallConvention().requiresPrologEpilogSave(*it))
               {
                  m_w[1] << "pop, " << m_t.getProc().getRegName(*it);
                  m_w.advanceLine();
               }
            }
            m_w[1] << "pop, " << m_t.getProc().getRegName(cmn::tgt::kStorageStackFramePtr);
            m_w.advanceLine();
            m_w[1] << "ret";
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kLabel:
         {
            m_w[0] << i.getArgs()[0]->getName() << ":";
            m_w.advanceLine();
         }
         break;
      // no ops: any space has already be calculated and
      //         reserved by kEnterFunc
      case cmn::tgt::kReserveLocal:
      case cmn::tgt::kUnreserveLocal:
      case cmn::tgt::kRet: // the jump emitted by early return transform is enough
         break;
      case cmn::tgt::kGlobalConstData:
         {
            m_w[0] << i.comment << ":";
            m_w.advanceLine();
            m_w[0] << ".data, " << i.getArgs()[0]->getName();
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kCall:
      case cmn::tgt::kSyscall:
         {
            m_w[1] << m_t.getProc().getInstr(i.instrId)->name << ",";
            asmArgWriter(m_v,m_t,m_w).write(i);
            handleComment(i);
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kPreCallStackAlloc:
         handlePrePostCallStackAlloc(i,cmn::tgt::kSub);
         break;
      case cmn::tgt::kPostCallStackAlloc:
         handlePrePostCallStackAlloc(i,cmn::tgt::kAdd);
         break;
      default:
         {
            m_w[1] << m_t.getProc().getInstr(i.instrId)->name << ",";
            asmArgWriter(m_v,m_t,m_w).write(i);
            handleComment(i);
            m_w.advanceLine();
         }
   }
}

void asmCodeGen::handlePrePostCallStackAlloc(lirInstr& i, cmn::tgt::instrIds x)
{
   size_t size = i.getArgs()[0]->getSize();
   if(size == 0)
      m_w[1] << "; ";

   m_w[1] << m_t.getProc().getInstr(x)->name
      << ", " << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr)
      << ", " << size;

   handleComment(i);
   m_w.advanceLine();
}

void asmCodeGen::handleComment(lirInstr& i)
{
   if(!i.comment.empty())
      m_w[2] << "; " << i.comment;
}

} // namespace liam
