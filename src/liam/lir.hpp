#pragma once
#include "../cmn/target.hpp"
#include "../cmn/type.hpp"
#include <list>
#include <map>
#include <set>
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
namespace cmn { class outStream; }
namespace cmn { class textTableLineWriter; }

namespace liam {

class lirStreams;

class lirArg {
public:
   lirArg() : disp(0), addrOf(false) {}
   virtual ~lirArg() {}

   virtual const std::string& getName() const = 0;
   virtual size_t getSize() const = 0;
   virtual lirArg& clone() const = 0;

   int disp;
   bool addrOf;

protected:
   virtual lirArg& copyFieldsInto(lirArg& noob) const;
};

class lirArgVar : public lirArg { // named var
public:
   lirArgVar(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual const std::string& getName() const { return name; }
   virtual size_t getSize() const { return m_size; }
   virtual lirArg& clone() const { return copyFieldsInto(*new lirArgVar(name,m_size)); }

private:
   const size_t m_size;
};

class lirArgConst : public lirArg {
public:
   lirArgConst(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual const std::string& getName() const { return name; }
   virtual size_t getSize() const { return m_size; }
   virtual lirArg& clone() const { return copyFieldsInto(*new lirArgConst(name,m_size)); }

private:
   const size_t m_size;
};

class lirArgTemp : public lirArg { // private var
public:
   lirArgTemp(const std::string& name, size_t size) : name(name), m_size(size) {}

   std::string name;

   virtual const std::string& getName() const { return name; }
   virtual size_t getSize() const { return m_size; }
   virtual lirArg& clone() const { return copyFieldsInto(*new lirArgTemp(name,m_size)); }

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
   lirInstr& injectBefore(lirInstr& noob);
   lirInstr& injectAfter(lirInstr& noob);
   lirInstr& append(lirInstr& noob);

   size_t orderNum;

   const cmn::tgt::instrIds instrId;
   const cmn::tgt::instrFmt *pInstrFmt;

   std::string comment;

   bool isLast() const { return m_pNext == NULL; }
   lirInstr& next() { return *m_pNext; }
   lirInstr& head();
   lirInstr& tail();
   lirInstr& search(size_t orderNum); // rename search down
   lirInstr& searchUp(cmn::tgt::instrIds id);

   std::vector<lirArg*>& getArgs() { return m_args; }

//private:
   explicit lirInstr(const cmn::tgt::instrIds id);
private:

   lirInstr *m_pPrev;
   lirInstr *m_pNext;
   std::vector<lirArg*> m_args;
};

class lirStream {
public:
   lirStream() : pTail(NULL), pTop(NULL) {}
   ~lirStream();

   std::string name;

   lirInstr *pTail;
   lirStreams *pTop;
   std::string segment;

private:
   std::map<lirArg*,lirInstr*> m_temps;
};

class lirStreams {
public:
   lirStream& addNewObject(const std::string& name, const std::string& segment);

   //std::map<std::string,lirStream> page; // keep string, but make a list
   std::list<lirStream> objects;
};

class lirFormatter {
public:
   lirFormatter(cmn::outStream& s, cmn::tgt::iTargetInfo& t) : m_s(s), m_t(t) {}

   void format(lirStreams& s);

private:
   void format(lirStream& s);
   void format(lirInstr& i, cmn::textTableLineWriter& t);
   void format(lirArg& a, cmn::textTableLineWriter& t);

   cmn::outStream& m_s;
   cmn::tgt::iTargetInfo& m_t;
};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//
// NEW LIR API
//
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

class instrBuilder;
class instructionless;

// maybe instead it's
//
// m_lGen
//    .forNode(n)              // returns 'instructionless' API
//    .append(cmn::tgt::kCall) // returns instr API
//    .addArg()                // returns arg
//
// you can re-run forNode() to add more

class lirGenerator { // lirBuilder?
public:
   lirGenerator(lirStreams& lir, cmn::tgt::iTargetInfo& t);

   // ditch this?
   void createNewStream(const std::string& segment, const std::string& comment);

   instrBuilder append(cmn::node& n, cmn::tgt::instrIds id);

   instructionless noInstr(cmn::node& n);

private:
   void bindArg(cmn::node& n, lirArg& a);
   void addArgFromNode(cmn::node& n, lirInstr& i);

   lirStreams& m_lir;
   cmn::tgt::iTargetInfo& m_t;
   lirStream *m_pCurrStream;
   std::set<std::string> m_nameTable;
   std::map<cmn::node*,lirArg*> m_nodeTable;
   std::set<lirArg*> m_adoptedOrphans; // TODO delete these

friend class instrBuilder;
friend class instructionless;
};

class instrBuilder {
public:
   instrBuilder(lirGenerator& g, cmn::tgt::iTargetInfo& t, lirInstr& i, cmn::node& n)
   : m_g(g), m_t(t), m_i(i), m_n(n) {}

   instrBuilder& withArg(lirArg& a)
   {
      m_i.addArg(a);
      return *this;
   }

   template<class T>
   instrBuilder& withArg(const std::string& name, size_t s)
   {
      m_i.addArg<T>(name,s);
      return *this;
   }

   template<class T>
   instrBuilder& withArg(const std::string& name, cmn::node& n)
   {
      auto nSize = cmn::type::gNodeCache->demand(n).getPseudoRefSize();
      m_i.addArg<T>(name,nSize);
      return *this;
   }

   instrBuilder& inheritArgFromChild(cmn::node& n) //?method name follows a different pattern
   { m_g.addArgFromNode(n,m_i); return *this; }

   instrBuilder& withComment(const std::string& c)
   { m_i.comment = c; return *this; }

   instrBuilder& returnToParent(size_t nArg)
   { m_g.bindArg(m_n,*m_i.getArgs()[nArg]); return *this; }

private:
   lirGenerator& m_g;
   cmn::tgt::iTargetInfo& m_t;
   lirInstr& m_i;
   cmn::node& m_n;
};

class instructionless {
public:
   instructionless(lirGenerator& g, cmn::node& n) : m_g(g), m_n(n) {}

   const lirArg& borrowArgFromChild(cmn::node& n);

   instructionless& returnToParent(lirArg& a);

private:
   lirGenerator& m_g;
   cmn::node& m_n;
};

} // namespace liam
