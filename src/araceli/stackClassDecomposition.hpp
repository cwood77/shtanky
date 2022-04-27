#pragma once
#include "../cmn/ast.hpp"
#include <list>
#include <utility>

namespace araceli {

class stackClassDecomposition : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::localDeclNode& n);

   void inject();

private:
   void inject(cmn::localDeclNode& local, const std::string& className);

   std::list<std::pair<cmn::localDeclNode*,std::string> > m_stackClasses;
};

} // namespace araceli
