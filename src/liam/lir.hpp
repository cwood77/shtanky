#pragma once
#include "../cmn/target.hpp"
#include <string>
#include <vector>

namespace liam {

class lirArg {
public:
   virtual ~lirArg() {}
   virtual size_t getSize(size_t targetPtrWidth) const = 0;
};

class lirArgVar : public lirArg {
public:
   lirArgVar(const std::string& name, size_t size);

   std::string name;

private:
   virtual size_t getSize(size_t targetPtrWidth) const;
};

class lirArgConst : public lirArg {
public:
   lirArgConst(const std::string& name, size_t size);

   std::string name;

private:
   virtual size_t getSize(size_t targetPtrWidth) const;
};

class lirInstr {
public:
   explicit lirInstr(const cmn::tgt::instrIds instrId);
   virtual ~lirInstr();

   static lirInstr *append(lirInstr*& pPrev);
   void addArg(lirArg& a);

   //size_t orderNum;

   const cmn::tgt::instrIds instrId;
   //const cmn::tgt::instrFmt *pInstrFmt;

   /*
   std::map<size_t,lirVar*> m_usage;
   std::map<lirVar*,size_t> m_refUsage;
   */

   bool isLast() const;
   lirInstr& next();

   std::vector<lirArg*>& getArgs() { return m_args; }

private:
   lirInstr *m_pPrev;
   lirInstr *m_pNext;
   std::vector<lirArg*> m_args;
};

/*
class lirVar {
public:
   std::string id;
   size_t firstAccess;
   size_t lastAccess;
   size_t numAccesses;
   size_t size;

   size_t global;
   size_t stackSlot;

   size_t targetStorage;
};
*/

} // namespace liam
