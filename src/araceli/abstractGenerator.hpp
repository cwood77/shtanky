#pragma once
#include "../cmn/ast.hpp"
#include <map>

namespace araceli {

class classInfo {
public:
   std::map<std::string,cmn::methodNode*> lastMethod;
   std::map<std::string,std::string> vTable;
   std::set<std::string> abstracts;
};

class classInfos {
public:
   std::map<std::string,classInfo> catalog;
};

// find all abstract methods of a given class, and it's bases
// implement those to throw errors
class abstractGenerator : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);

   classInfos m_infos;

private:
   void populateVTable(cmn::classNode& n);
};

} // namespace araceli
