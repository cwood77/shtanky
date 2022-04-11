#pragma once
#include "../cmn/ast.hpp"
#include <list>

namespace cmn { class outStream; }

namespace araceli {

class batGen : public cmn::araceliVisitor<> {
public:
   explicit batGen(cmn::outStream& s) : m_s(s) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::araceliProjectNode& n);
   virtual void visit(cmn::fileNode& n);

private:
   cmn::outStream& m_s;
   std::list<std::string> m_files;
};

} // namespace araceli
