#include "ast.hpp"

namespace cmn {

void iNodeVisitor::visitChildren(node& n)
{
   auto v = n.getChildren();
   for(auto it=v.begin();it!=v.end();++it)
      (*it)->acceptVisitor(*this);
}

node::~node()
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
      delete *it;
}

void node::appendChild(node& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}

} // namespace cmn
