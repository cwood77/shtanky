#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <string>
#include <vector>

// generally, classInfo is a snapshot of the AST the user wrote--i.e. soon after parse.
// classInfo is NOT kept up-to-date throughout transformations; the rule is, you may
// ADD data, but don't bother EDITING existing data.
//
// classInfo is gathered _after_ object is added as the implied base class, but _before_
// all other transformations.  objectBaser is an exception because it participates in
// symbol linking (i.e. load).

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
   classInfo();

   void addOrReplaceVirtualMethod(cmn::classNode& forClass, cmn::methodNode& n);

   std::string name;
   std::string fqn;
   //size_t flags;
   cmn::classNode *pNode;
   cmn::classNode *pVTableClass;
   std::list<std::string> bases;

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
