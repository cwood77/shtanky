#include "ast.hpp"

namespace cmn {

class writeMarker : public hNodeVisitor {
public:
   virtual void visit(assignmentNode& n)
   {
      hNodeVisitor::visit(n);
      n.getChildren()[0]->flags |= nodeFlags::kAddressableForWrite;
   }

   virtual void _implementLanguage() {} // all
};

} // namespace cmn
