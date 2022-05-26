#pragma once
#include "../araceli/symbolTable.hpp"

namespace salome {

class nodeLinker : public araceli::nodeLinker {
protected:
   virtual bool typePropLink(cmn::node& root, cmn::symbolTable& sTable);
   virtual bool tryFixUnresolved(cmn::node& root, cmn::symbolTable& sTable);
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
   virtual bool decomposesLoopsDuringLink() { return true; }
};

} // namespace salome
