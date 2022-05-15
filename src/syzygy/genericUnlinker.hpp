#pragma once
#include "../cmn/ast.hpp"
#include "../cmn/symbolTable.hpp"

namespace syzygy {

class genericUnlinker : public cmn::hNodeVisitor {
public:
   explicit genericUnlinker(cmn::symbolTable& st)
   : nChanges(0), m_sTable(st), m_insideGeneric(false) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::genericNode& n);

   virtual void _implementLanguage() {} // all

   size_t nChanges;

private:
   void ignore(cmn::linkBase& l);

   cmn::symbolTable& m_sTable;
   bool m_insideGeneric;
};

} // namespace syzygy
