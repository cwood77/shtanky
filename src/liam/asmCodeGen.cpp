#include "../cmn/out.hpp"
#include "../cmn/target.hpp"
#include "../cmn/textTable.hpp"
#include "asmCodeGen.hpp"
#include "lir.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"

namespace liam {

void asmArgWriter::write(lirInstr& i, size_t firstArg, size_t nArgs)
{
   auto& args = i.getArgs();
   size_t j=0;
   size_t w=0;
   if(!nArgs)
      nArgs = args.size();
   for(auto it=args.begin();it!=args.end()&&w<nArgs;++it,j++)
   {
      if(j>=firstArg)
      {
         write(i.orderNum,**it);
         w++;
      }
   }
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
      m_w[1] << "["
         << m_t.getProc().getRegName(framePtr)
         << (cmn::tgt::getStackDisp(stor) + a.disp)
      << "]";
   else if(a.addrOf)
   {
      m_w[1] << "[" << m_t.getProc().getRegName(stor);
      writeDispIf(a);
      m_w[1] << "]";
   }
   else
   {
      if(stor == cmn::tgt::kStorageImmediate)
         m_w[1] << a.getName();
      else
         m_w[1] << m_t.getProc().getRegName(stor);
      writeDispIf(a);
   }

   m_first = false;
}

void asmArgWriter::writeDispIf(lirArg& a)
{
   if(a.disp)
   {
      if(a.disp > 0)
         m_w[1] << "+";
      m_w[1] << a.disp;
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
            auto& regs = m_f.getUsedRegs();
            for(auto it=regs.begin();it!=regs.end();++it)
            {
               if(m_t.getCallConvention().requiresPrologEpilogSave(*it))
               {
                  m_w[1] << "push, " << m_t.getProc().getRegName(*it);
                  m_w.advanceLine();
               }
            }
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
            size_t locals = m_f.getUsedStackSpace();
            if(locals)
            {
               m_w[1]
                  << "add, "
                  << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr) << ", "
                  << locals;
               m_w.advanceLine();
            }
            auto& regs = m_f.getUsedRegs();
            for(auto it=regs.rbegin();it!=regs.rend();++it)
            {
               if(m_t.getCallConvention().requiresPrologEpilogSave(*it))
               {
                  m_w[1] << "pop, " << m_t.getProc().getRegName(*it);
                  m_w.advanceLine();
               }
            }
            m_w[1] << "ret";
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kReserveLocal:
         {
            m_w[1]
               << "sub, "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr) << ", "
               << i.getArgs()[0]->getSize(); // this size is already real
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kUnreserveLocal:
         {
            m_w[1]
               << "add, "
               << m_t.getProc().getRegName(cmn::tgt::kStorageStackPtr) << ", "
               << i.getArgs()[0]->getSize(); // this size is already real
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kGlobalConstData:
         {
            m_w[0] << i.comment << ":";
            m_w.advanceLine();
            m_w[0] << ".data, " << i.getArgs()[0]->getName();
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kPush:
      case cmn::tgt::kPop:
      case cmn::tgt::kMov:
      case cmn::tgt::kRet:
         {
            m_w[1] << m_t.getProc().getInstr(i.instrId)->name << ",";
            asmArgWriter(m_v,m_t,m_w).write(i);
            handleComment(i);
            m_w.advanceLine();
         }
         break;
      case cmn::tgt::kCall:
      case cmn::tgt::kSyscall:
         {
            m_w[1] << m_t.getProc().getInstr(i.instrId)->name << ",";
            asmArgWriter(m_v,m_t,m_w).write(i,1,1);
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
         throw std::runtime_error("unknown instruction in codegen!");
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
