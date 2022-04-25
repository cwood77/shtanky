#pragma once
#include "../cmn/ast.hpp"
#include <set>

namespace cmn { class outBundle; }

namespace araceli2 {

class fileRefs {
public:
   void addRef(const std::string& path) { m_paths.insert(path); }

   void flush(std::ostream& stream);

   std::string destPath;

private:
   std::set<std::string> m_paths;
};

class fileRefCollector {
public:
   fileRefCollector() : m_pRefs(NULL) {}

   void bind(fileRefs *pR) { m_pRefs = pR; }
   void onLink(cmn::linkBase& l);

private:
   fileRefs *m_pRefs;
};

class liamTypeWriter : public cmn::araceliVisitor<> {
public:
   liamTypeWriter(std::ostream& o, fileRefCollector& refs) : m_o(o), m_refs(refs) {}

   virtual void visit(cmn::node& n) {}
   virtual void visit(cmn::typeNode& n);
   virtual void visit(cmn::strTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);

private:
   std::ostream& m_o;
   fileRefCollector& m_refs;
};

class codeGen : public cmn::araceliVisitor<> {
public:
   explicit codeGen(cmn::outBundle& out) : m_pActiveFile(NULL), m_out(out) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::sequenceNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::localDeclNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::assignmentNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   void generateClassType(cmn::classNode& n, cmn::outStream& header, const std::string& vname);

   void generateCallFromOpenParen(cmn::node& n, bool skipFirst);

   cmn::fileNode *m_pActiveFile;
   cmn::outBundle& m_out;

   fileRefs m_hRefs;
   fileRefs m_sRefs;

   fileRefCollector m_refColl;
};

} // namespace araceli
