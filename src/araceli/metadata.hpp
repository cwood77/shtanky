#pragma once
#include "../cmn/ast.hpp"
#include <list>
#include <map>

namespace araceli {

class metadata {
public:
   void publish(const std::string& tag, cmn::node& n);
   void demandMulti(const std::string& tag, std::list<cmn::node*>& n);

private:
   std::map<std::string,std::list<cmn::node*> > m_map;
};

class nodeMetadataBuilder : public cmn::araceliVisitor<> { // maybe this is generic?
public:
   explicit nodeMetadataBuilder(metadata& m) : m_m(m) {}

   virtual void visit(cmn::node& n);

private:
   metadata& m_m;
};

} // namespace araceli
