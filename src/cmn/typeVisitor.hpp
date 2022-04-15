#pragma once
#include "ast.hpp"
#include "type.hpp"

// these visitors are very minimalistic right now
// core careabout was being able to calculate displacements
// for field access nodes; that requires knowing class types
// and doing basic type prop in the fine AST

namespace cmn {

class typeBuilderVisitor : public hNodeVisitor {
public:
   type::iType& getType() { return m_pBuilder->finish(); }

   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(ptrTypeNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

class coarseTypeVisitor : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(classNode& n);
   virtual void visit(fieldNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

class fineTypeVisitor : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }

   virtual void visit(strTypeNode& n);
   virtual void visit(fieldAccessNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace cmn
