#pragma once
#include "../cmn/ast.hpp"
#include <set>

namespace cmn { class outBundle; }

namespace araceli {

class iTarget;

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
   virtual void visit(cmn::boolTypeNode& n);
   virtual void visit(cmn::intTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::ptrTypeNode& n);

private:
   std::ostream& m_o;
   fileRefCollector& m_refs;
};

class codeGenBase : public cmn::araceliVisitor<> {
public:
   const std::string& getPath() const { return m_refs.destPath; }

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);

protected:
   codeGenBase(cmn::outBundle& out, const std::string& path);

   void generatePrototype(cmn::funcNode& n);
   void writeAttributes(cmn::node& n);
   virtual void appendFileSuffix() {}

   cmn::outStream *m_pOut;
   fileRefs m_refs;
   fileRefCollector m_refColl;
};

class headerCodeGen : public codeGenBase {
public:
   headerCodeGen(cmn::outBundle& out, const std::string& path) : codeGenBase(out,path) {}

   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::funcNode& n);
};

class sourceCodeGen : public codeGenBase {
public:
   sourceCodeGen(
      cmn::outBundle& out,
      const std::string& headerPath,
      const std::string& sourcePath,
      iTarget& tgt) : codeGenBase(out,sourcePath), m_headerPath(headerPath), m_tgt(tgt) {}

   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::sequenceNode& n);
   virtual void visit(cmn::returnNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::localDeclNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::assignmentNode& n);
   virtual void visit(cmn::bopNode& n);
   virtual void visit(cmn::ifNode& n);
   virtual void visit(cmn::forLoopNode& n);
   virtual void visit(cmn::whileLoopNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);
   virtual void visit(cmn::structLiteralNode& n);

protected:
   virtual void appendFileSuffix();

private:
   void generateCallFromOpenParen(cmn::node& n, bool skipFirst);
   void generateCommaDelimitedChildren(cmn::node& n, bool skipFirst = false);

   std::string m_headerPath;
   iTarget& m_tgt;
};

class codeGen : public cmn::araceliVisitor<> {
public:
   explicit codeGen(iTarget& tgt, cmn::outBundle& out) : m_tgt(tgt), m_out(out) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);

private:
   iTarget& m_tgt;
   cmn::outBundle& m_out;
};

} // namespace araceli
