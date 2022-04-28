#pragma once
#include <list>

namespace cmn { class uniquifier; }

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class lirStreams;
class varTable;

class lirTransform {
public:
   virtual ~lirTransform();
   virtual void runStreams(lirStreams& lir);
   virtual void runStream(lirStream& s);

protected:
   lirTransform() : m_pCurrStream(NULL) {}
   virtual void runInstr(lirInstr& i);
   virtual void runArg(lirInstr& i, lirArg& a) {}

   void scheduleInjectBefore(lirInstr& noob, lirInstr& before);
   void scheduleInjectAfter(lirInstr& noob, lirInstr& after);
   void scheduleAppend(lirInstr& noob);

   lirStream& getCurrentStream() { return *m_pCurrStream; }

private:
   class iChange {
   public:
      virtual ~iChange() {}
      virtual void apply() = 0;
   };

   class injectChange : public iChange {
   public:
      injectChange(lirInstr& noob, lirInstr& antecedent, bool beforeAfter, lirStream& s)
      : m_pNoob(&noob), m_antecedent(antecedent), m_beforeAfter(beforeAfter), m_s(s) {}
      ~injectChange() { delete m_pNoob; }

      virtual void apply();

   private:
      lirInstr *m_pNoob;
      lirInstr& m_antecedent;
      bool m_beforeAfter;
      lirStream& m_s;
   };

   class appendChange : public iChange {
   public:
      appendChange(lirInstr& noob, lirStream& s) : m_pNoob(&noob), m_s(s) {}
      ~appendChange() { delete m_pNoob; }

      virtual void apply();

   private:
      lirInstr *m_pNoob;
      lirStream& m_s;
   };

   void applyChanges();

   std::list<iChange*> m_changes;
   lirStream *m_pCurrStream;
};

// not so much a transform as a helper for populating the name table
class lirNameCollector : public lirTransform {
public:
   explicit lirNameCollector(cmn::uniquifier& u) : m_u(u) {}

protected:
   virtual void runArg(lirInstr& i, lirArg& a);

private:
   cmn::uniquifier& m_u;
};

class lirCallVirtualStackCalculation : public lirTransform {
public:
   explicit lirCallVirtualStackCalculation(cmn::tgt::iTargetInfo& t)
   : m_t(t), m_pPreCall(NULL) {}

protected:
   virtual void runInstr(lirInstr& i);
   virtual void runArg(lirInstr& i, lirArg& a);

private:
   cmn::tgt::iTargetInfo& m_t;
   lirInstr *m_pPreCall;
   std::vector<size_t> m_argRealSizes;
};

// inject the following instrs:
//    kSelectSegment
//    kExitFunc
//    kUnreserveLocal
//    kPostCallStackAlloc
//
class lirPairedInstrDecomposition : public lirTransform {
protected:
   virtual void runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);
};

class lirCodeShapeDecomposition : public lirTransform {
protected:
   virtual void runInstr(lirInstr& i);
};

class lirNumberingTransform : public lirTransform {
public:
   lirNumberingTransform() : m_next(10) {}

protected:
   virtual void runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);

private:
   unsigned long m_next;
};

void runLirTransforms(lirStreams& lir, cmn::tgt::iTargetInfo& t);

class lirVarGen : public lirTransform {
public:
   explicit lirVarGen(varTable& v) : m_v(v) {}

protected:
   virtual void runArg(lirInstr& i, lirArg& a);

private:
   varTable& m_v;
};

// some instructions (e.g. call) have arguments only to enforce calling convention storage
// requirements; they aren't actually passed at a machine code level.  These need to be
// stripped before asm gen.
class spuriousVarStripper : public lirTransform {
protected:
   virtual void runInstr(lirInstr& i);
};

class codeShapeTransform : public lirTransform {
public:
   codeShapeTransform(varTable& v, cmn::tgt::iTargetInfo& t) : m_v(v), m_t(t) {}

protected:
   virtual void runInstr(lirInstr& i);

private:
   void Register(lirArg& a, std::vector<cmn::tgt::argTypes>& at);
   void immediate(lirArg& a, std::vector<cmn::tgt::argTypes>& at);
   void memory(lirArg& a, std::vector<cmn::tgt::argTypes>& at);

   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam
