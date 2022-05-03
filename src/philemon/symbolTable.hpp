#pragma once
#include "../araceli/symbolTable.hpp"

namespace philemon {

class nodeLinker : public araceli::nodeLinker {
protected:
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace philemon
