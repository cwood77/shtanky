#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <string>
#include <vector>

namespace cmn { class outStream; }

namespace araceli {

class methodInfo {
public:
   methodInfo() : pMethod(NULL) {}

   std::string name;
   std::string fqn;
   size_t flags;
   cmn::methodNode *pMethod;
   cmn::methodNode *pBaseImpl;
   //cmn::funcNode *pAsFunc;
   bool inherited;
};

class classInfo {
public:
//   classInfo();

   void addOrReplaceVirtualMethod(cmn::classNode& forClass, cmn::methodNode& n);

   std::string name;
   std::string fqn;
   //size_t flags;
   cmn::classNode *pNode;

   std::vector<methodInfo> inheritedAndDirectMethods;
   std::map<std::string,size_t> nameLookup;
};

class classCatalog {
public:
   classInfo& create(cmn::classNode& n);
   classInfo& get(cmn::classNode& n);

   std::map<std::string,classInfo> classes;
};

class classInfoBuilder : public cmn::araceliVisitor<> {
public:
   explicit classInfoBuilder(classCatalog& cc) : m_cCat(cc) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);

private:
   classCatalog& m_cCat;
};

class classInfoFormatter {
public:
   explicit classInfoFormatter(cmn::outStream& s) : m_s(s) {}

   void format(classCatalog& cc);

private:
   cmn::outStream& m_s;
};

} // namespace araceli
