#pragma once
#include "../araceli/symbolTable.hpp"

namespace salome {

class nodeLinker : public araceli::nodeLinker {
protected:
   virtual bool tryFixUnresolved(cmn::node& root, cmn::symbolTable& sTable);
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace salome
