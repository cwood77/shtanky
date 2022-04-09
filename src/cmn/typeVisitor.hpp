#pragma once
#include "ast.hpp"
#include "type.hpp"

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
   virtual void visit(funcNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

} // namespace cmn
