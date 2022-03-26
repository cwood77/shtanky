#pragma once
#include "../cmn/target.hpp"
#include <map>
#include <string>
#include <vector>

// backend compile
// - number instrs
// - determine var lifetime (using numbers)
// - each var solicits requirements from clients
// - create a register bank from the target
// - step over each instruction
//   - for each living variable, choose a slot
// - select an overload for each instruction
// - write

namespace cmn { class node; }

namespace liam {

class lirArg {
public:
   virtual ~lirArg() {}
   virtual size_t getSize() const = 0;
   virtual void dump() const = 0;
};

class lirArgVar : public lirArg {
public:
   lirArgVar(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual size_t getSize() const { return m_size; }
   virtual void dump() const;

private:
   const size_t m_size;
};

class lirArgConst : public lirArg {
public:
   lirArgConst(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual size_t getSize() const { return m_size; }
   virtual void dump() const;

private:
   const size_t m_size;
};

class lirInstr {
public:
   virtual ~lirInstr();

   static lirInstr& append(lirInstr*& pPrev, const cmn::tgt::instrIds id);
   void addArg(lirArg& a) { m_args.push_back(&a); }

   void dump();

   size_t orderNum;

   const cmn::tgt::instrIds instrId;
   const cmn::tgt::instrFmt *pInstrFmt;

   bool isLast() const { return m_pNext == NULL; }
   lirInstr& next() { return *m_pNext; }
   lirInstr& head();

   std::vector<lirArg*>& getArgs() { return m_args; }

private:
   explicit lirInstr(const cmn::tgt::instrIds id);

   lirInstr *m_pPrev;
   lirInstr *m_pNext;
   std::vector<lirArg*> m_args;
};

class lirVarStorage {
public:
   static lirVarStorage reg(size_t s);
   static lirVarStorage stack(int offset);

   int stackOffset;
   size_t targetStorage;
};

class lirVar {
public:
   lirVar();

   std::string id;
   size_t firstAccess;
   size_t lastAccess;
   size_t numAccesses;
   size_t size;

   size_t global;

   std::map<size_t,lirVarStorage> storage;
};

class lirStream {
public:
   lirStream() : pTail(NULL), m_nTemp(0) {}
   ~lirStream();

   void dump();

   lirInstr *pTail;

   lirArg& createNamedArg(/*cmn::node& n,*/ lirInstr& i, const std::string& name, size_t size);
   //lirArg& findNamedArg(cmn::node& n, const std::string& name);

   // donations are inlined into callers, and are never variables
   template<class T>
   void donate(cmn::node& n, const std::string& value, size_t size)
   { _donate(n,*new T(value,size)); }

   template<class T>
   void createTemporary(cmn::node& n, lirInstr& i, const std::string& value, size_t size)
   { _createTemporary(n,i,*new T(value,size)); }

   lirArg& claimArg(cmn::node& n, lirInstr& i);

   lirVar& getVariableByName(const std::string& name);
   std::vector<lirVar*> getVariablesInScope(size_t instrOrderNum);

private:
   void _donate(cmn::node& n, lirArg& a);
   void _createTemporary(cmn::node& n, lirInstr& i, lirArg& a);

   size_t m_nTemp;
   std::map<lirArg*,lirInstr*> m_temps;
   std::map<cmn::node*,lirArg*> m_nodeOutputs;
   std::map<std::string,lirVar> m_varTable;
};

class lirStreams {
public:
   void dump();

   std::map<std::string,lirStream> page;
};

} // namespace liam
