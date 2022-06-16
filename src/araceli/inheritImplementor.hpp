#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class classCatalog;

class fieldInitializer : public cmn::hNodeVisitor {
public:
   fieldInitializer(cmn::node& seq, const std::string& fname)
   : m_seq(seq), m_fname(fname) {}

   virtual void visit(cmn::strTypeNode& n) { unexpected(n); }
   virtual void visit(cmn::boolTypeNode& n);
   virtual void visit(cmn::intTypeNode& n) { setToZero(); }
   virtual void visit(cmn::arrayTypeNode& n) { unexpected(n); }
   virtual void visit(cmn::voidTypeNode& n) { setToZero(); }
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::ptrTypeNode& n) { setToZero(); }

   virtual void _implementLanguage() {} // all

private:
   void setToZero();

   cmn::node& m_seq;
   std::string m_fname;
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
