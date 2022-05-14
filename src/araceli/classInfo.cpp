#include "../cmn/out.hpp"
#include "../cmn/userError.hpp"
#include "classInfo.hpp"

namespace araceli {

methodInfo::methodInfo()
: flags(0), pMethod(NULL), pBaseImpl(NULL), inherited(false)
{
}

classInfo::classInfo()
: pNode(NULL)
{
}

void classInfo::addOrReplaceVirtualMethod(cmn::classNode& forClass, cmn::methodNode& n)
{
   auto it = nameLookup.find(n.name);
   if(it == nameLookup.end())
   {
      inheritedAndDirectMethods.push_back(methodInfo());
      nameLookup[n.name] = inheritedAndDirectMethods.size() - 1;
   }

   methodInfo& mi = inheritedAndDirectMethods[nameLookup[n.name]];
   mi.name = n.name;
   mi.fqn = cmn::fullyQualifiedName::build(n,n.name);
   mi.flags = n.flags;
   mi.pBaseImpl = mi.pMethod;
   mi.pMethod = &n;
   mi.inherited = (&forClass != &n.getAncestor<cmn::classNode>());

   if(mi.pBaseImpl == NULL &&
      !(mi.flags & cmn::nodeFlags::kAbstract) &&
      !(mi.flags & cmn::nodeFlags::kVirtual))
      cmn::gUserErrors->add(n,cmn::kErrorMethodsStartingInheritanceMustBeAbstractOrVirtual);
   if(mi.pBaseImpl != NULL &&
      !(mi.flags & cmn::nodeFlags::kOverride))
      cmn::gUserErrors->add(n,cmn::kErrorOverridesMustUseKeyword);

   // inherit the access
   if(mi.pBaseImpl)
   {
      size_t specB = (mi.pBaseImpl->flags & cmn::nodeFlags::kAccessSpecifierMask);
      mi.pMethod->flags |= specB;
      mi.flags |= specB;
   }
}

void classInfo::checkNonVirtualMethod(cmn::methodNode& n)
{
   auto it = nameLookup.find(n.name);
   if(it != nameLookup.end())
      cmn::gUserErrors->add(n,cmn::kErrorNonVirtualMethodHidesInheritedMethod);
}

classInfo& classCatalog::create(cmn::classNode& n)
{
   auto fqn = cmn::fullyQualifiedName::build(n);
   classInfo& info = classes[fqn];
   info.name = n.name;
   info.fqn = fqn;
   info.flags = n.flags;
   info.pNode = &n;
   auto l = n.computeLineage();
   for(auto it=l.begin();it!=l.end();++it)
      info.bases.push_back(cmn::fullyQualifiedName::build(**it));
   info.bases.pop_back(); // do not include myself
   return info;
}

void classInfoBuilder::visit(cmn::classNode& n)
{
   auto& info = m_cCat.create(n);

   // build
   auto l = n.computeLineage();
   for(auto it=l.begin();it!=l.end();++it)
   {
      auto methods = (*it)->getChildrenOf<cmn::methodNode>();
      for(auto mit=methods.begin();mit!=methods.end();++mit)
         if((*mit)->flags & (cmn::nodeFlags::kVirtual | cmn::nodeFlags::kOverride | cmn::nodeFlags::kAbstract))
            info.addOrReplaceVirtualMethod(n,**mit);
         else
            info.checkNonVirtualMethod(**mit);
   }

   // infer abstract
   for(auto it=info.inheritedAndDirectMethods.begin();it!=info.inheritedAndDirectMethods.end();++it)
   {
      if(it->flags & cmn::nodeFlags::kAbstract)
      {
         info.flags |= cmn::nodeFlags::kAbstract;
         break;
      }
   }
}

void classInfoFormatter::format(classCatalog& cc)
{
   m_s.stream() << "dumping class catalog" << std::endl;
   m_s.stream() << std::endl;

   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      m_s.stream() << "CLASS " << cit->second.fqn << std::endl;
      cmn::autoIndent _i(m_s);
      m_s.stream() << cmn::indent(m_s) << "name: " << cit->second.name << std::endl;
      m_s.stream() << cmn::indent(m_s) << "fqn: " << cit->second.fqn << std::endl;
      m_s.stream() << cmn::indent(m_s) << "flags: " << cit->second.flags << std::endl;
      m_s.stream() << std::endl;

      for(auto mit=cit->second.inheritedAndDirectMethods.begin();mit!=cit->second.inheritedAndDirectMethods.end();++mit)
      {
         cmn::autoIndent _i(m_s);
         m_s.stream() << cmn::indent(m_s) << "METHOD " << mit->name << std::endl;
         m_s.stream() << cmn::indent(m_s) << "fqn: " << mit->fqn << std::endl;
         m_s.stream() << cmn::indent(m_s) << "flags: " << mit->flags << std::endl;
         m_s.stream() << cmn::indent(m_s) << "pMethod: " << (size_t)(mit->pMethod) << std::endl;
         m_s.stream() << cmn::indent(m_s) << "pBaseImpl: " << (size_t)(mit->pBaseImpl) << std::endl;
         m_s.stream() << cmn::indent(m_s) << "inherited?: " << (mit->inherited ? "T" : "F") << std::endl;
         m_s.stream() << std::endl;
      }
   }
}

} // namespace araceli
