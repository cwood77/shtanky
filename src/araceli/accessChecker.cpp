#include "../cmn/stlutil.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/type.hpp"
#include "../cmn/userError.hpp"
#include "accessChecker.hpp"
#include "classInfo.hpp"

namespace araceli {

void memberAccessChecker::visit(cmn::invokeNode& n)
{
   auto instTypeName = cmn::type::gNodeCache->demand(*n.getChildren()[0]).getName();
   auto& cInfo = cmn::demand(m_cc.classes,instTypeName,"class not found");
   auto idx = cmn::demand(cInfo.nameLookup,n.proto.ref,"class does not have this method");
   size_t access = cInfo.inheritedAndDirectMethods[idx].pMethod->flags
      & cmn::nodeFlags::kAccessSpecifierMask;

   enforce(n,*cInfo.pNode,n.getAncestor<cmn::classNode>(),access);

   hNodeVisitor::visit(n);
}

void memberAccessChecker::visit(cmn::fieldAccessNode& n)
{
   hNodeVisitor::visit(n);
}

void memberAccessChecker::enforce(cmn::node& site, cmn::classNode& refee, cmn::classNode& refer, size_t kind)
{
   cdwDEBUG("checking %s -> %s at %lu with kind %lld\n",
      refer.name.c_str(), refee.name.c_str(), site.lineNumber, kind);

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
