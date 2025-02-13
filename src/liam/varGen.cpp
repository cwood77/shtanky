#include "../cmn/out.cpp"
#include "../cmn/throw.cpp"
#include "../cmn/trace.cpp"
#include "lir.hpp"
#include "varGen.hpp"
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace liam {

const lirArg& var::lastArg()
{
   return **(--((--(refs.end()))->second.end()));
}

size_t var::getSize()
{
   return (*refs.begin()->second.begin())->getSize();
}

// remove all references to the argument but keep storage (and orderNums).
// keeping storage is important for transforms that run after regalloc.
// otherwise, should you unbind an instruction that dictated storage (e.g. a call)
// _all_ other usages of the varible would also lose storage
void var::unbindArgButKeepStorage(lirInstr& i, lirArg& a)
{
   // refs
   {
      auto& s = refs[i.orderNum];
      {
         for(auto it=s.begin();it!=s.end();++it)
         {
            if(*it == &a)
            {
               s.erase(it);
               break;
            }
         }
      }
      if(s.size() == 0)
         refs.erase(i.orderNum);
   }

   // disAmbig
   {
      storageDisambiguators.erase(&a);
   }
}

bool var::isAlive(size_t orderNum)
{
   return refs.size() &&
      refs.begin()->first <= orderNum && orderNum <= (--(refs.end()))->first;
}

bool var::isAlive(size_t start, size_t end)
{
   return isAlive(start) || isAlive(end) ||
      (refs.size() && start < refs.begin()->first && (--(refs.end()))->first < end);
}

// ans    req
//  b  0
//  b  1  BX
//  b  2
//  d  3  DX   splitter will inject at 2.5 to mov BX <- BX
//  d  4
//
std::set<size_t> var::getStorageAt(size_t orderNum)
{
   if(instrToStorageMap.size() == 0)
      return std::set<size_t>();

   auto found = instrToStorageMap.begin();
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first <= orderNum)
         found = it;
      else
         break;

   return found->second;
}

size_t var::getStorageFor(size_t orderNum, lirArg& a)
{
   auto it = storageDisambiguators.find(&a);
   if(it != storageDisambiguators.end())
      return it->second;

   auto stors = getStorageAt(orderNum);
   if(stors.size() != 1)
      cdwTHROW("insane?  somehow arg %s on instr %lld has %lld storage(s)?",
         a.getName().c_str(),orderNum,stors.size());
   return *(stors.begin());
}

// whens the next (i.e. after 'orderNum') requirement on 'storage'?
size_t var::requiresStorageNext(size_t orderNum, size_t storage)
{
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first > orderNum)
         if(it->second.find(storage) != it->second.end())
            return it->first;

   return 0;
}

// was this variable's most previous storage this storage?
bool var::alreadyWantedStorage(size_t orderNum, size_t storage)
{
   std::set<size_t> lastStorage;
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first < orderNum)
         lastStorage = it->second;

   return lastStorage.find(storage)!=lastStorage.end();
}

// n.b. add a new requirement, but don't change existing requirements
void var::requireStorage(size_t orderNum, size_t s)
{
   instrToStorageMap[orderNum].insert(s);
   storageToInstrMap[s].insert(orderNum);
}

void var::changeStorage(size_t orderNum, size_t old, size_t nu)
{
   instrToStorageMap[orderNum].erase(old);
   instrToStorageMap[orderNum].insert(nu);
   storageToInstrMap[old].erase(orderNum);
   if(storageToInstrMap[old].size() == 0)
      storageToInstrMap.erase(old);
   storageToInstrMap[nu].insert(orderNum);

   for(auto it=storageDisambiguators.begin();it!=storageDisambiguators.end();++it)
      if(it->second == old)
         it->second = nu;
}

// why is this so different from changeStorage above?
// TODO - this seems highly questionable!!?
//        search all instructions for no good reason
//        if instr i doesn't have a storage req on old, add one for nu
void var::updateStorageHereAndAfter(lirInstr& i, size_t old, size_t nu)
{
   cdwDEBUG("--updateStorageHereAndAfter[%lld,%lld->%lld]\n",i.orderNum,old,nu);

   lirInstr *pInstr = &i;
   while(true)
   {
      auto it = instrToStorageMap.find(pInstr->orderNum);
      if(it!=instrToStorageMap.end() && it->second.find(old)!=it->second.end())
      {
         cdwDEBUG("   hit\n");
#if 0
         it->second.erase(old);
         it->second.insert(nu);

         storageToInstrMap[old].erase(pInstr->orderNum);
         if(storageToInstrMap[old].size() == 0)
            storageToInstrMap.erase(old);
         storageToInstrMap[nu].insert(pInstr->orderNum);

         auto& args = i.getArgs();
         for(auto jit=args.begin();jit!=args.end();++jit)
         {
            auto kit = storageDisambiguators.find(*jit);
            if(kit != storageDisambiguators.end() && kit->second == old)
               kit->second = nu;
         }
#endif
      }
      else
      {
         cdwDEBUG("   ?no hit\n");

         if(pInstr->orderNum == i.orderNum)
         {
            cdwDEBUG("      since this is instr %llu, inject a requirement\n",i.orderNum);
            requireStorage(i.orderNum,nu);
         }
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

void var::format(cmn::outStream& s)
{
   s.stream() << cmn::indent(s) << "name=" << name << std::endl;

   s.stream() << cmn::indent(s) << "refs = {" << std::endl;
   {
      cmn::autoIndent _i(s);
      for(auto it=refs.begin();it!=refs.end();++it)
         for(auto *pArg : it->second)
            s.stream()
               << cmn::indent(s) << "i" << it->first << ": "
               << lirIncrementalFormatter::argToString(*pArg)
               << "(" << (size_t)pArg << ")"
               << std::endl;
   }
   s.stream() << cmn::indent(s) << "}" << std::endl;

   s.stream() << cmn::indent(s) << "instrToStorageMap = {" << std::endl;
   {
      cmn::autoIndent _i(s);
      for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
         for(auto stor : it->second)
            s.stream()
               << cmn::indent(s) << "i" << it->first << ": "
               << storageToString(stor)
               << std::endl;
   }
   s.stream() << cmn::indent(s) << "}" << std::endl;

   s.stream() << cmn::indent(s) << "storageToInstrMap = {" << std::endl;
   {
      cmn::autoIndent _i(s);
      for(auto it=storageToInstrMap.begin();it!=storageToInstrMap.end();++it)
         for(auto instr : it->second)
            s.stream()
               << cmn::indent(s) << storageToString(it->first) << ": "
               << "i" << instr
               << std::endl;
   }
   s.stream() << cmn::indent(s) << "}" << std::endl;

   s.stream() << cmn::indent(s) << "storageDisambiguators = {" << std::endl;
   {
      cmn::autoIndent _i(s);
      for(auto it=storageDisambiguators.begin();it!=storageDisambiguators.end();++it)
         s.stream()
            << cmn::indent(s) << lirIncrementalFormatter::argToString(*it->first)
            << "(" << (size_t)it->first << "): "
            << storageToString(it->second)
            << std::endl;
   }
   s.stream() << cmn::indent(s) << "}" << std::endl;
}

std::string var::storageToString(size_t s)
{
   switch(s)
   {
      case cmn::tgt::kStorageUnassigned:
         return "sUndecided";
      case cmn::tgt::kStorageStackPtr:
         return "sStackPtr";
      case cmn::tgt::kStorageStackFramePtr:
         return "sStackFramePtr";
      case cmn::tgt::kStorageImmediate:
         return "sImm";
      case cmn::tgt::kStorageUndecidedStack:
         return "sUndecidedStack";
      default:
         {
            std::stringstream stream;
            if(cmn::tgt::isStackStorage(s))
            {
               int d = cmn::tgt::getStackDisp(s);
               if(d > 0)
                  stream << "sStack[+" << d << "]";
               else
                  stream << "sStack[" << d << "]";
               return stream.str();
            }
            else if(cmn::tgt::isVStack(s))
            {
               int d = cmn::tgt::getVStackInt(s);
               if(d > 0)
                  stream << "sVStack[+" << d << "]";
               else
                  stream << "sVStack[" << d << "]";
               return stream.str();
            }
            else
            {
               stream << "r" << s;
               return stream.str();
            }
         }
   }
}

size_t virtStackTable::reserveVirtStorage(size_t real)
{
   size_t v = cmn::tgt::makeVStack(m_next++);
   m_map[v] = real;
   return v;
}

size_t virtStackTable::mapToReal(size_t virt)
{
   auto it = m_map.find(virt);
   if(it == m_map.end())
      cdwTHROW("unknown virtual stack storage %lld",virt);
   return it->second;
}

void virtStackTable::format(cmn::outStream& s)
{
   if(m_map.size()==0)
      return;

   s.stream() << cmn::indent(s) << "VIRTSTACKUNIMPLED!" << std::endl;
}

varTable::~varTable()
{
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      delete it->second;
}

var& varTable::create(const std::string& name)
{
   var*& pVar = m_vars[name];
   if(!pVar)
   {
      pVar = new var;
      pVar->name = name;
   }
   return *pVar;
}

var& varTable::demand(const std::string& name)
{
   var *pVar = m_vars[name];
   if(!pVar)
      throw std::runtime_error("missing var");
   return *pVar;
}

var& varTable::demand(lirArg& a)
{
   var *pAns = fetch(a);

   if(!pAns)
      cdwTHROW("variable not found!");

   return *pAns;
}

var *varTable::fetch(lirArg& a)
{
   std::vector<var*> ans;

   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      for(auto jit=it->second->refs.begin();jit!=it->second->refs.end();++jit)
         for(auto kit=jit->second.begin();kit!=jit->second.end();++kit)
            if(*kit == &a)
               ans.push_back(it->second);

   if(ans.size() == 1)
      return ans[0];
   else if(ans.size() == 0)
      return NULL;
   else
   {
      for(auto it=ans.begin();it!=ans.end();++it)
      {
         std::string storage = "???";
         auto jit=(*it)->storageDisambiguators.find(&a);
         if(jit!=(*it)->storageDisambiguators.end())
            storage = cmn::fmt("%lld",jit->second);
         cdwDEBUG("v = '%s', stor=%lld\n",(*it)->name.c_str(),storage.c_str());
      }
      cdwTHROW("INSANITY! lirArg %s has %lld variables associated?!",a.getName().c_str(),ans.size());
   }
}

void varTable::format(cmn::outStream& s)
{
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
   {
      s.stream() << cmn::indent(s) << "VAR " << it->first << std::endl;
      cmn::autoIndent _i(s);
      it->second->format(s);
      s.stream() << std::endl;
   }

   m_vSTable.format(s);
}

} // namespace liam
