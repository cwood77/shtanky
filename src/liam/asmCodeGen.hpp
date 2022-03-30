#pragma once

namespace cmn { class outStream; }
namespace cmn { class textTableLineWriter; }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirInstr;
class lirStream;
class varTable;

class asmCodeGen {
public:
   static void generate(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, cmn::outStream& o);

private:
   static void generateArgs(lirInstr& i, varTable& v, cmn::tgt::iTargetInfo& t, cmn::textTableLineWriter& w);
};

} // namespace liam
