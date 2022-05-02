#pragma once
#include "../cmn/ast.hpp"
#include "iTarget.hpp"
#include <list>

namespace cmn { class outStream; }

namespace araceli {

class batGen : public cmn::araceliVisitor<> {
public:
   batGen(iTarget& tgt, cmn::outStream& s) : m_tgt(tgt), m_s(s) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::araceliProjectNode& n);
   virtual void visit(cmn::fileNode& n);

private:
   void adjustFiles(iTarget::phase p, const std::string ext, std::list<std::string>& l);

   iTarget& m_tgt;
   cmn::outStream& m_s;
   std::list<std::string> m_files;
};

} // namespace araceli
