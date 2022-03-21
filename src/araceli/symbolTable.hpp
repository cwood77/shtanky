#pragma once
#include "ast.hpp"

namespace araceli {

// ns1::ns2::classC + a::field
// possible answers
// baseClass's blassClass
// ::ns1::ns2::a::field
// ::ns1::a::field
// ::a::field
// class's have multiple scopes: their canonical scope *and* their inheritance chain

class symbolTable {
public:
   //std::map<std::string,cmn::node*> m_resolved;
   std::set<linkBase*> unresolved;

   void tryResolve(const std::string& refingScope, linkBase& l);
};

// knows about scope-providing nodes
class fullScopeNameBuilder : public iNodeVisitor {
public:
   std::string fqn;
};

// knows all the scopes of a given node
class linkResolver : public iNodeVisitor {
public:
   linkResolver(symbolTable& st, linkBase& l) : m_sTable(st), m_l(l) {}

private:
   symbolTable& m_sTable;
   linkBase& m_l;
};

// knows all the links in a given node
class nodeResolver : public iNodeVisitor {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

private:
   symbolTable& m_sTable;
};

} // namespace araceli
