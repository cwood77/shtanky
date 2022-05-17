#include "../cmn/stlutil.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/type.hpp"
#include "../cmn/userError.hpp"
#include "accessChecker.hpp"
#include "classInfo.hpp"

namespace araceli {

void memberAccessChecker::visit(cmn::invokeNode& n)
{
   enforce(
      n,
      n.proto.getRefee()->attributes,
      n.proto.getRefee()->getAncestor<cmn::classNode>(),
      n.getAncestor<cmn::classNode>(),
      n.proto.getRefee()->flags & cmn::nodeFlags::kAccessSpecifierMask);

   hNodeVisitor::visit(n);
}

void memberAccessChecker::visit(cmn::fieldAccessNode& n)
{
   auto instTypeName = cmn::type::gNodeCache->demand(*n.getChildren()[0]).getName();
   auto& cInfo = cmn::demand(m_cc.classes,instTypeName,"class not found");
   cmn::fieldNode *pField = NULL;
   auto lineage = cInfo.pNode->computeLineage();
   for(auto it=lineage.rbegin();it!=lineage.rend();++it)
   {
      auto hits = (*it)->filterChildren<cmn::fieldNode>([&](auto& f){ return f.name == n.name; });
      if(hits.size())
      {
         pField = *hits.begin();
         break;
      }
   }
   if(!pField)
      cdwTHROW("instance does not have field '%s'",n.name.c_str());
   size_t access = pField->flags & cmn::nodeFlags::kAccessSpecifierMask;

   enforce(n,pField->attributes,pField->getAncestor<cmn::classNode>(),n.getAncestor<cmn::classNode>(),access);

   hNodeVisitor::visit(n);
}

void memberAccessChecker::enforce(cmn::node& site, std::set<std::string>& attributes, cmn::classNode& refee, cmn::classNode& refer, size_t kind)
{
   cdwDEBUG("checking %s -> %s at %lu with kind %lld\n",
      refer.name.c_str(), refee.name.c_str(), site.lineNumber, kind);

   std::string attr = "allow:" + refer.name;
   cdwDEBUG("  (looking for magic attribute '%s')\r\n",attr.c_str());
   if(attributes.find(attr)!=attributes.end())
   {
      cdwDEBUG("   allowed b/c of magic attribute!\r\n");
      return;
   }

   if(kind == cmn::nodeFlags::kPublic)
      return;
   else if(kind == cmn::nodeFlags::kProtected)
   {
      if(!isAnAncestor(refee,refer))
         cmn::gUserErrors->add(site,cmn::kErrorIllegalUseOfMember) << "kind is protected";
   }
   else if(kind == cmn::nodeFlags::kPrivate)
   {
      if(&refee != &refer)
         cmn::gUserErrors->add(site,cmn::kErrorIllegalUseOfMember) << "kind is private";
   }
   else
      cdwTHROW("ISE");
}

// is refer same or derived from refee?
bool memberAccessChecker::isAnAncestor(cmn::classNode& refee, cmn::classNode& refer)
{
   auto ancestors = refer.computeLineage(); // n.b. lineage includes self
   for(auto *pAn : ancestors)
      if(pAn == &refee)
         return true;

   return false;
}

} // namespace araceli
