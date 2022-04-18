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

class lirStreams;

class lirArg {
public:
   lirArg() : disp(0), addrOf(false) {}
   virtual ~lirArg() {}

   virtual size_t getSize() const = 0;
   virtual void dump() const = 0;

   int disp;
   bool addrOf;
};

class lirArgVar : public lirArg { // named var
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

class lirArgTemp : public lirArg {
public:
   lirArgTemp(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual size_t getSize() const { return m_size; }
   virtual void dump() const;

private:
   const size_t m_size;
};

class lirInstr {
public:
   virtual ~lirInstr();

   static lirInstr& append(lirInstr*& pPrev, const cmn::tgt::instrIds id, const std::string& comment);

   template<class T>
   T& addArg(const std::string& n, size_t z)
   {
      T *pArg = new T(n,z);
      addArg(*pArg);
      return *pArg;
   }

   lirArg& addArg(lirArg& a) { m_args.push_back(&a); return a; }

   lirInstr& injectBefore(const cmn::tgt::instrIds id, const std::string& comment);

   void dump();

   size_t orderNum;

   const cmn::tgt::instrIds instrId;
   const cmn::tgt::instrFmt *pInstrFmt;

   std::string comment;

   bool isLast() const { return m_pNext == NULL; }
   lirInstr& next() { return *m_pNext; }
   lirInstr& head();
   lirInstr& search(size_t orderNum);

   std::vector<lirArg*>& getArgs() { return m_args; }

private:
   explicit lirInstr(const cmn::tgt::instrIds id);

   lirInstr *m_pPrev;
   lirInstr *m_pNext;
   std::vector<lirArg*> m_args;
};

class lirStream {
public:
   lirStream() : pTail(NULL), pTop(NULL) {}
   ~lirStream();

   void dump();

   lirInstr *pTail;
   lirStreams *pTop;

private:
   std::map<lirArg*,lirInstr*> m_temps;
};

class lirStreams {
public:
   void dump();

   // after the first instr on a page is added,
   // pick a good initial order num that doesn't
   // overlap
   // ... so don't write to multiple pages at once!
   void onNewPageStarted(const std::string& key);

   std::map<std::string,lirStream> page;
};

} // namespace liam
