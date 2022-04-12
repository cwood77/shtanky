#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class constHoister : public cmn::araceliVisitor<> {
public:
   constHoister() : m_pFile(NULL), m_next(0) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::stringLiteralNode& n);

private:
   cmn::fileNode *m_pFile;
   size_t m_next;
};

} // namespace araceli
