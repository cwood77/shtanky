#pragma once
#include "../araceli/symbolTable.hpp"

namespace philemon {

class nodeLinker : public araceli::nodeLinker {
protected:
   virtual bool typePropLink(cmn::node& root, cmn::symbolTable& sTable);
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace philemon
