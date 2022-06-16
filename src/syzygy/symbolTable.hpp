#pragma once

namespace cmn { class node; }
namespace cmn { class symbolTable; }

namespace syzygy {

class nodeLinkerHelper {
public:
   static bool typePropLink(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace syzygy
