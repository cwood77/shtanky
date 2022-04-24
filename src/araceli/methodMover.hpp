#pragma once
#include "../cmn/ast.hpp"
#include <list>

namespace araceli {

class methodMover : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::methodNode& n);

private:
   std::list<cmn::methodNode*> m_methods;
};

} // namespace araceli
