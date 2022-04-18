#pragma once
#include "ast.hpp"
#include "type.hpp"

// these visitors are very minimalistic right now
// core careabout was being able to calculate displacements
// for field access nodes; that requires knowing class types
// and doing basic type prop in the fine AST

namespace cmn {

// seed the graph with types for obvious terminals: type nodes and literal nodes
class builtInTypeVisitor : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }

   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(ptrTypeNode& n);

   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

// build-up classes from built-in types (and other classes)
class userTypeVisitor : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(classNode& n);
   virtual void visit(fieldNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

// build-up functions
class functionVisitor : public hNodeVisitor {
public:
   functionVisitor() : m_pClass(NULL) {}

   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(funcNode& n);
   virtual void visit(argNode& n);

   virtual void _implementLanguage() {} // all

private:
   classNode *m_pClass;
   std::unique_ptr<type::typeBuilder> m_pBuilder;
};

// slide types around on the wires
class typePropagator : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(invokeNode& n);
   virtual void visit(invokeFuncPtrNode& n);
   virtual void visit(fieldAccessNode& n);
   virtual void visit(callNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(bopNode& n);

   virtual void _implementLanguage() {} // all
};

void propagateTypes(cmn::node& root);

} // namespace cmn
