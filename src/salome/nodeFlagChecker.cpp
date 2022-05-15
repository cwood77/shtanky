#include "../cmn/userError.hpp"
#include "nodeFlagChecker.hpp"

namespace salome {

void nodeFlagChecker::visit(cmn::node& n)
{
   if(n.flags)
      cmn::gUserErrors->add(n,cmn::kErrorFlagsIllegalOnNode) << n.flags << " on cmn::node";
   visitChildren(n);
}

void nodeFlagChecker::visit(cmn::classNode& n)
{
   size_t illegal = (n.flags & ~(cmn::nodeFlags::kAbstract | cmn::nodeFlags::kInterface));
   if(illegal)
      cmn::gUserErrors->add(n,cmn::kErrorFlagsIllegalOnNode) << illegal << " on class";

   visitChildren(n);
}

void nodeFlagChecker::visit(cmn::memberNode& n)
{
   size_t i = 0;
   if(n.flags & cmn::nodeFlags::kPublic) i++;
   if(n.flags & cmn::nodeFlags::kProtected) i++;
   if(n.flags & cmn::nodeFlags::kPrivate) i++;
   if(i > 1)
      cmn::gUserErrors->add(n,cmn::kErrorTooManyAccessSpecifiers);
}

void nodeFlagChecker::visit(cmn::methodNode& n)
{
   hNodeVisitor::visit(n);

   size_t i = 0;
   if(n.flags & cmn::nodeFlags::kVirtual) i++;
   if(n.flags & cmn::nodeFlags::kOverride) i++;
   if(n.flags & cmn::nodeFlags::kAbstract) i++;
   if(i > 1)
   {
      cmn::gUserErrors->add(n,cmn::kErrorTooManyDynDispatchFlags);
      i = 1;
   }
   if(n.flags & cmn::nodeFlags::kStatic) i++;
   if(i > 1)
      cmn::gUserErrors->add(n,cmn::kErrorStaticCantBeUsedWithDynDispatch);

   if(n.flags & cmn::nodeFlags::kInterface)
      cmn::gUserErrors->add(n,cmn::kErrorFlagsIllegalOnNode) << "interface on method";

   if(
      (n.flags & cmn::nodeFlags::kAccessSpecifierMask) && 
      (n.flags & cmn::nodeFlags::kOverride)
   )
      cmn::gUserErrors->add(n,cmn::kErrorOverrideCannotSpecifyAccess);

   // set the default access scope if unspecified
   if(!(n.flags & cmn::nodeFlags::kAccessSpecifierMask))
   {
      if(n.flags & (cmn::nodeFlags::kAbstract | cmn::nodeFlags::kVirtual))
         n.flags |= cmn::nodeFlags::kProtected;
      else if(!(n.flags & cmn::nodeFlags::kOverride))
         n.flags |= cmn::nodeFlags::kPrivate;
      // leave override defaults to the classinfo
   }

   visitChildren(n);
}

void nodeFlagChecker::visit(cmn::fieldNode& n)
{
   hNodeVisitor::visit(n);

   if(n.flags & (
      cmn::nodeFlags::kVirtual |
      cmn::nodeFlags::kOverride |
      cmn::nodeFlags::kAbstract |
      cmn::nodeFlags::kInterface |
      cmn::nodeFlags::kPublic |
      cmn::nodeFlags::kProtected
      ))
      cmn::gUserErrors->add(n,cmn::kErrorFlagsIllegalOnNode) << n.flags << " on field";

   if(n.name.c_str()[0] != '_')
      cmn::gUserErrors->add(n,cmn::kErrorFieldsBeginWithUnderscore)
         << "actual name: " << n.name;

   // set the default access scope if unspecified
   if(!(n.flags & cmn::nodeFlags::kAccessSpecifierMask))
      n.flags |= cmn::nodeFlags::kPrivate;

   visitChildren(n);
}

} // namespace salome
