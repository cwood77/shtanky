#pragma once

namespace cmn { class outStream; }
namespace cmn { class textTableLineWriter; }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class varFinder;
class varTable;

class asmArgWriter {
public:
   asmArgWriter(varTable& v, cmn::tgt::iTargetInfo& t, cmn::textTableLineWriter& w)
   : m_v(v), m_t(t), m_w(w), m_first(true) {}

   void write(lirInstr& i);

private:
   void write(size_t orderNum, lirArg& a);
   void writeDispIf(const __int64& disp);

   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
   cmn::textTableLineWriter& m_w;
   bool m_first;
};

class asmCodeGen {
public:
   static void generate(lirStream& s, varTable& v, varFinder& f, cmn::tgt::iTargetInfo& t, cmn::outStream& o);

private:
   asmCodeGen(varTable& v, varFinder& f, cmn::tgt::iTargetInfo& t, cmn::textTableLineWriter& w)
   : m_v(v), m_f(f), m_t(t), m_w(w) {}

   void handleInstr(lirInstr& i);
   void handlePrePostCallStackAlloc(lirInstr& i, cmn::tgt::instrIds x);
   void handleComment(lirInstr& i);

   varTable& m_v;
   varFinder& m_f;
   cmn::tgt::iTargetInfo& m_t;
   cmn::textTableLineWriter& m_w;
};

} // namespace liam
