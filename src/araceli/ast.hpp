#include "../cmn/ast.hpp"

namespace araceli {

class classNode;
class fileNode;

class iNodeVisitor : public cmn::iNodeVisitor {
public:
   virtual void visit(classNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
};

class classNode : public cmn::node {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class fileNode : public cmn::node {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

} // namespace araceli
