#pragma once
#include "../cmn/ast.hpp"
#include "classInfo.hpp"
#include <list>

namespace araceli {

class methodMover : public cmn::araceliVisitor<> {
public:
   explicit methodMover(classCatalog& cc) : m_cc(cc) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::methodNode& n);

private:
   classCatalog& m_cc;
   std::list<cmn::methodNode*> m_methods;
};

} // namespace araceli
