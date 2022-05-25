#pragma once
#include "availVarPass.hpp"
#include <list>

namespace cmn { class uniquifier; }

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class lirStreams;
class varFinder;
class varTable;

class lirTransform {
public:
   virtual ~lirTransform();
   virtual void runStreams(lirStreams& lir);
   void runStream(lirStream& s);

protected:
   lirTransform() : m_pCurrStream(NULL) {}

   virtual void _runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);
   virtual void runArg(lirInstr& i, lirArg& a) {}

   void scheduleInjectBefore(lirInstr& noob, lirInstr& before);
   void scheduleInjectAfter(lirInstr& noob, lirInstr& after);
   void scheduleAppend(lirInstr& noob);
   void scheduleVarBind(lirInstr& noob, lirArg& arg, varTable& v, size_t storage);

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

   class varBindChange : public iChange {
   public:
      varBindChange(lirInstr& noob, lirArg& arg, varTable& v, size_t storage)
      : m_i(noob), m_a(arg), m_v(v), m_stor(storage) {}

      virtual void apply();

   private:
      lirInstr& m_i;
      lirArg& m_a;
      varTable& m_v;
      size_t m_stor;
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
   : m_t(t) {}

protected:
   virtual void runInstr(lirInstr& i);
   virtual void runArg(lirInstr& i, lirArg& a);

private:
   cmn::tgt::iTargetInfo& m_t;
   std::list<lirInstr*> m_preCalls;
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
   virtual void _runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);
};

// implement the following instrs:
//    kMacroIfFalse
class lirBranchDecomposition : public lirTransform {
protected:
   virtual void runInstr(lirInstr& i);
};

// TODO there's two of these--one pre regalloc and one post
//      combine?  or just rename them?
class lirCodeShapeDecomposition : public lirTransform {
protected:
   virtual void runInstr(lirInstr& i);
};

class lirNumberingTransform : public lirTransform {
public:
   lirNumberingTransform() : m_next(10) {}

protected:
   virtual void _runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);

private:
   unsigned long m_next;
};

// TODO is this a good idea?  Prolly not
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
public:
   explicit spuriousVarStripper(varTable& v) : m_v(v) {}

protected:
   virtual void runInstr(lirInstr& i);

private:
   varTable& m_v;
};

class codeShapeTransform : public lirTransform {
public:
   codeShapeTransform(varTable& v, varFinder& f, cmn::tgt::iTargetInfo& t)
   : m_v(v), m_f(f), m_t(t), m_vfProg(v,f) {}

protected:
   virtual void runInstr(lirInstr& i);

private:
   void categorizeArgs(lirInstr& i, std::vector<cmn::tgt::argTypes>& at);
   void markReg(lirArg& a, std::vector<cmn::tgt::argTypes>& at);
   void markImm(lirArg& a, std::vector<cmn::tgt::argTypes>& at);
   void markMem(lirArg& a, std::vector<cmn::tgt::argTypes>& at);

   void getInstrInfo(
      lirInstr& i,
      const std::vector<cmn::tgt::argTypes>& at,
      const cmn::tgt::instrInfo*& pIInfo,
      bool& needsReshaping);

   void findWorkingInstrFmt(
      const cmn::tgt::instrInfo& iInfo,
      const std::vector<cmn::tgt::argTypes>& args,
      std::vector<cmn::tgt::argTypes>& retryArgs,
      std::set<size_t>& changedIndicies);
   bool mutateInstrFmt(
      const cmn::tgt::instrInfo& iInfo,
      const std::vector<cmn::tgt::argTypes>& args,
      std::vector<cmn::tgt::argTypes>& retryArgs,
      std::set<size_t>& changedIndicies);

   bool isReadOnly(const cmn::tgt::instrInfo& info, size_t idx);
   bool isMemory(cmn::tgt::argTypes a);
   cmn::tgt::argTypes makeRegister(cmn::tgt::argTypes a);
   bool isStackFramePtrInUse(
      lirInstr& i,
      const cmn::tgt::instrInfo& iInfo,
      std::vector<cmn::tgt::argTypes> origArgs);

   varTable& m_v;
   varFinder& m_f;
   cmn::tgt::iTargetInfo& m_t;
   varFinderProgrammer m_vfProg;
};

} // namespace liam
