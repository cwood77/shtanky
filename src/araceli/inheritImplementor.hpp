#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class classCatalog;

class fieldInitializer : public cmn::hNodeVisitor {
public:
   explicit fieldInitializer(cmn::node& seq) : m_seq(seq) {}

   virtual void visit(cmn::strTypeNode& n) { unexpected(n); }
   virtual void visit(cmn::boolTypeNode& n);
   virtual void visit(cmn::intTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n) { unexpected(n); }
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::ptrTypeNode& n);

   virtual void _implementLanguage() {} // all

private:
   cmn::node& m_seq;
};

class inheritImplementor {
public:
   void generate(classCatalog& cc);

private:
   void combineFieldsAndRemoveBases(classCatalog& cc);
   void initializeFields(classCatalog& cc);
   void addVPtrs(classCatalog& cc);
};

} // namespace araceli
