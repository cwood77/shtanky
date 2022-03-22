#include "metadata.hpp"

namespace araceli {

void metadata::publish(const std::string& tag, cmn::node& n)
{
   m_map[tag].push_back(&n);
}

void metadata::demandMulti(const std::string& tag, std::list<cmn::node*>& n)
{
   n = m_map[tag];
}

void nodeMetadataBuilder::visit(cmn::node& n)
{
   for(auto it=n.attributes.begin();it!=n.attributes.end();++it)
      m_m.publish(*it,n);
}

} // namespace araceli
