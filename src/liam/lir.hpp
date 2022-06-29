#pragma once
#include "../cmn/autoDump.hpp"
#include "../cmn/target.hpp"
#include "../cmn/type.hpp"
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace cmn { class node; }
namespace cmn { class outStream; }
namespace cmn { class textTableLineWriter; }

namespace liam {

class lirStreams;

class lirArg {
public:
   virtual ~lirArg() {}

   const std::string& getName() const { return m_name; }
   // sizes here are typically pseudo, but some instrs differ
   size_t getSize() const { return m_size; }
   virtual lirArg& clone() const = 0;

   int disp;
   bool addrOf;

protected:
   lirArg(const std::string& name, size_t size)
   : disp(0), addrOf(false), m_name(name), m_size(size) {}

   template<class T>
   T& _clone() const
   {
      T& dup = *new T(m_name,m_size);
      copyFieldsInto(dup);
      return dup;
   }

   virtual lirArg& copyFieldsInto(lirArg& noob) const;

private:
   const std::string m_name;
   const size_t m_size;
};

class lirArgVar : public lirArg {
public:
   lirArgVar(const std::string& name, size_t size) : lirArg(name,size) {}
   virtual lirArg& clone() const { return _clone<lirArgVar>(); }
};
class lirArgConst : public lirArg {
public:
   lirArgConst(const std::string& name, size_t size) : lirArg(name,size) {}
   virtual lirArg& clone() const { return _clone<lirArgConst>(); }
};
class lirArgLabel : public lirArgConst {
public:
   lirArgLabel(const std::string& name, size_t size)
   : lirArgConst(name,size), isCode(false) {}
   bool isCode;
   virtual lirArg& clone() const { return _clone<lirArgLabel>(); }
protected:
   virtual lirArg& copyFieldsInto(lirArg& noob) const;
};
class lirArgTemp : public lirArg {
public:
   lirArgTemp(const std::string& name, size_t size) : lirArg(name,size) {}
   virtual lirArg& clone() const { return _clone<lirArgTemp>(); }
};

class lirInstr {
public:
   explicit lirInstr(const cmn::tgt::instrIds id);
   virtual ~lirInstr();

   template<class T>
   T& addArg(const std::string& n, size_t z)
   {
      T *pArg = new T(n,z);
      addArg(*pArg);
      return *pArg;
   }
   lirArg& addArg(lirArg& a) { m_args.push_back(&a); return a; }

   lirInstr& injectBefore(lirInstr& noob);
   lirInstr& injectAfter(lirInstr& noob);
   lirInstr& append(lirInstr& noob);

   bool isLast() const { return m_pNext == NULL; }
   lirInstr& prev() { return *m_pPrev; }
   lirInstr& next() { return *m_pNext; }
   lirInstr& head();
   lirInstr& tail();
   lirInstr& searchUp(std::function<bool(const lirInstr&)> pred);

   size_t orderNum;
   cmn::tgt::instrIds instrId;
   std::string comment;
   std::vector<lirArg*>& getArgs() { return m_args; }

private:
   void pickOrderNumForNewLocation();

   lirInstr *m_pPrev;
   lirInstr *m_pNext;
   std::vector<lirArg*> m_args;
};

class lirStream {
public:
   lirStream() : pTail(NULL) {}
   ~lirStream();

   std::string name;

   lirInstr *pTail;
   std::string segment;
};

class lirStreams {
public:
   lirStream& addNewObject(const std::string& name, const std::string& segment);

   std::list<lirStream> objects;
};

class lirIncrementalFormatter {
public:
   static std::string argToString(lirArg& a);

   lirIncrementalFormatter(cmn::outStream& s, cmn::tgt::iTargetInfo& t) : m_s(s), m_t(t) {}

   void start(lirStreams& s);
   void format(lirStream& s);
   void end();

private:
   void _format(lirStream& s);
   void format(lirInstr& i, cmn::textTableLineWriter& t);
   void appendTargetHints();

   cmn::outStream& m_s;
   cmn::tgt::iTargetInfo& m_t;
};

class lirFormatter {
public:
   lirFormatter(cmn::outStream& s, cmn::tgt::iTargetInfo& t) : m_s(s), m_t(t) {}

   void format(lirStreams& s);

private:
   cmn::outStream& m_s;
   cmn::tgt::iTargetInfo& m_t;
};

class lirAutoLogger : public cmn::iLogger {
public:
   lirAutoLogger(lirStreams& lir, cmn::tgt::iTargetInfo& t)
   : m_lir(lir), m_pStream(NULL), m_tgt(t) {}

   virtual std::string getExt() { return ".lir"; }

   virtual void dump(cmn::outStream& s)
   {
      if(m_pStream)
      { lirIncrementalFormatter(s,m_tgt).format(*m_pStream); }
      else
      { lirFormatter(s,m_tgt).format(m_lir); }
   }

   void setIncremental(lirStream *pStream) { m_pStream = pStream; }

private:
   lirStreams& m_lir;
   lirStream *m_pStream;
   cmn::tgt::iTargetInfo& m_tgt;
};

class autoIncrementalSetting {
public:
   explicit autoIncrementalSetting(lirAutoLogger& l, lirStream& s)
   : m_lirLogger(l), m_stream(s)
   { m_lirLogger.setIncremental(&s); }

   ~autoIncrementalSetting()
   { m_lirLogger.setIncremental(NULL); }

private:
   lirAutoLogger& m_lirLogger;
   lirStream& m_stream;
};

class lirBuilder {
public:
   class nodeScope;
   class instrBuilder;

   lirBuilder(lirStreams& lir, cmn::tgt::iTargetInfo& t)
   : m_lir(lir), m_t(t), m_pCurrStream(NULL), m_nLabel(0) {}

   ~lirBuilder();

   void createNewStream(const std::string& name, const std::string& segment);
   std::string reserveNewLabel(const std::string& nameHint);

   nodeScope forNode(cmn::node& n) { return nodeScope(*this,n); }

   const lirArg& borrowArgFromChild(cmn::node& n);

   class nodeScope {
   public:
      nodeScope(lirBuilder& b, cmn::node& n) : m_b(b), m_n(n) {}

      instrBuilder append(cmn::tgt::instrIds i);

      nodeScope& returnToParent(lirArg& a);

   private:
      lirBuilder& m_b;
      cmn::node& m_n;
   };

   class instrBuilder {
   public:
      instrBuilder(lirBuilder& b, cmn::node& n, lirInstr& i) : m_b(b), m_n(n), m_i(i) {}

      instrBuilder& withComment(const std::string& c)
      { m_i.comment = c; return *this; }

      instrBuilder& withArg(lirArg& a) { m_i.addArg(a); return *this; }

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

      instrBuilder& inheritArgFromChild(cmn::node& n)
      { m_b.addArgFromNode(n,m_i); return *this; }

      instrBuilder& returnToParent(size_t nArg)
      { m_b.publishArg(m_n,*m_i.getArgs()[nArg]); return *this; }

      template<class T>
      T& tweakArgAs(size_t nArg)
      { return dynamic_cast<T&>(*m_i.getArgs()[nArg]); }

      lirInstr& instr() { return m_i; }

      nodeScope then() { return m_b.forNode(m_n); }

   private:
      lirBuilder& m_b;
      cmn::node& m_n;
      lirInstr& m_i;
   };

private:
   void publishArg(cmn::node& n, lirArg& a);
   void adoptArg(cmn::node& n, lirArg& a);
   void addArgFromNode(cmn::node& n, lirInstr& i);

   lirStreams& m_lir;
   cmn::tgt::iTargetInfo& m_t;
   lirStream *m_pCurrStream;
   size_t m_nLabel;
   std::map<cmn::node*,lirArg*> m_cache;
   std::set<lirArg*> m_orphans;
};

} // namespace liam
