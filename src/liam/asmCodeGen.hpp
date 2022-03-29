#pragma once

namespace cmn { class columnedOutStream; }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirInstr;
class lirStream;
class varTable;

class asmCodeGen {
public:
   static void generate(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, cmn::columnedOutStream& o);

private:
   static void generateArgs(lirInstr& i, varTable& v, cmn::tgt::iTargetInfo& t, std::stringstream& s);
};

} // namespace liam
