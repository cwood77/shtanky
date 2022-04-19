#pragma once
#include <list>

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class lirStreams;

class lirTransform {
public:
   virtual ~lirTransform();
   virtual void runStreams(lirStreams& lir);

protected:
   lirTransform() : m_pCurrStream(NULL) {}
   virtual void runStream(lirStream& s);
   virtual void runInstr(lirInstr& i);
   virtual void runArg(lirArg& a);

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
      injectChange(lirInstr& noob, lirInstr& antecedent, bool beforeAfter)
      : m_pNoob(&noob), m_antecedent(antecedent), m_beforeAfter(beforeAfter) {}
      ~injectChange() { delete m_pNoob; }

      virtual void apply();

   private:
      lirInstr *m_pNoob;
      lirInstr& m_antecedent;
      bool m_beforeAfter;
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

void runLirTransforms(lirStreams& lir);

} // namespace liam
