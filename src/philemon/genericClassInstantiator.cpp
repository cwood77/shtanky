#include "../cmn/commonLexor.hpp"
#include "../cmn/commonParser.hpp"
#include "../cmn/nameUtil.hpp"
#include "../cmn/stlutil.hpp"
#include "../cmn/trace.hpp"
#include "genericClassInstantiator.hpp"
#include <cstring>

namespace philemon {

void typeParamedNameResolver::visit(cmn::classNode& n)
{
   resolveText(n.name);

   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
      resolveText(it->ref);

   hNodeVisitor::visit(n);
}

void typeParamedNameResolver::visit(cmn::userTypeNode& n)
{
   resolveText(n.pDef.ref);

   if(n.pDef.ref == "str")
      registerReplacement(n,*new cmn::strTypeNode());
   // this is the only one for now

   hNodeVisitor::visit(n);
}

void typeParamedNameResolver::visit(cmn::invokeNode& n)
{
   resolveText(n.proto.ref);
   hNodeVisitor::visit(n);
}

void typeParamedNameResolver::visit(cmn::callNode& n)
{
   resolveText(n.pTarget.ref);
   hNodeVisitor::visit(n);
}

void typeParamedNameResolver::finish()
{
   for(auto it=m_replaces.begin();it!=m_replaces.end();++it)
   {
      auto& old = *it->first;
      auto& nu = *it->second;

      nu.lineNumber = old.lineNumber;
      nu.attributes = old.attributes;
      nu.flags = old.flags;
      nu.getChildren() = old.getChildren();

      delete old.getParent()->replaceChild(old,nu);
   }

   m_replaces.clear();
}

void typeParamedNameResolver::resolveText(std::string& s)
{
   // see if the entire string is a key
   {
      auto it = m_dict.find(s);
      if(it != m_dict.end())
      {
         cdwVERBOSE("replacing '%s' -> '%s'\n",s.c_str(),it->second.c_str());
         s = it->second;
         return;
      }
   }

   // see if the string is a type expression
   const char *pLt = ::strchr(s.c_str(),'<');
   if(pLt == NULL)
      return;

   // parse it
   std::string base;
   std::list<std::string> args;
   {
      cmn::genericTypeExprLexor l(s.c_str());
      cmn::genericTypeExprParser p(l);
      p.parseTypeParamed(base,args);
   }

   // resolve each
   for(auto it=args.begin();it!=args.end();++it)
      resolveText(*it);

   // rebuild
   std::stringstream stream;
   stream << base << "<";
   for(auto it=args.begin();it!=args.end();++it)
   {
      if(it!=args.begin())
         stream << ",";
      stream << *it;
   }
   stream << ">";
   cdwVERBOSE("replacing '%s' -> '%s'\n",s.c_str(),stream.str().c_str());
   s = stream.str();
}

void typeParamedNameResolver::registerReplacement(cmn::node& old, cmn::node& nu)
{
   m_replaces.push_back(std::make_pair<cmn::node*,cmn::node*>(&old,&nu));
}

void instantiationFinder::visit(cmn::fileNode& n)
{
   // note generic classes
   auto gs = n.getChildrenOf<cmn::genericNode>();
   for(auto it=gs.begin();it!=gs.end();++it)
   {
      auto& rClass = (*it)->demandSoleChild<cmn::classNode>();
      cmn::addUnique(genericsByName,cmn::fullyQualifiedName::build(rClass),*it);
   }

   // note instantiate statements
   auto is = n.getChildrenOf<cmn::instantiateNode>();
   for(auto it=is.begin();it!=is.end();++it)
      if(!(*it)->impled)
         unimpled.insert(*it);
}

void instantiationFinder::dump()
{
   cdwVERBOSE("found %lld generics\n",genericsByName.size());
   cdwVERBOSE("found %lld unimpled instantiate statements\n",unimpled.size());
}

size_t classInstantiator::run(cmn::node& n)
{
   cdwVERBOSE("searching for instantiate statements\n");

   instantiationFinder iFinder;
   n.acceptVisitor(iFinder);
   iFinder.dump();

   m_genericsByName = iFinder.genericsByName;
   for(auto it=iFinder.unimpled.begin();it!=iFinder.unimpled.end();++it)
      makeInstance(**it);

   return iFinder.unimpled.size();
}

void classInstantiator::makeInstance(cmn::instantiateNode& n)
{
   cdwVERBOSE("instantiating '%s'\n",n.text.c_str());
   std::string shortName;
   cmn::nameUtil::stripLast(n.text,shortName);

   // determine arguments
   std::string base;
   std::list<std::string> args;
   {
      cmn::genericTypeExprLexor l(n.text.c_str());
      cmn::genericTypeExprParser p(l);
      p.parseTypeParamed(base,args);
   }

   // locate template
   cmn::genericNode *pTemplate = m_genericsByName[base];
   if(!pTemplate)
      cdwTHROW("can't locate generic '%s' referened by instantiate '%s' on line %lu\n",
         base.c_str(),
         n.text.c_str(),
         n.lineNumber);

   // build dictionary
   std::map<std::string,std::string> dict;
   buildBinding(args,*pTemplate,dict);

   // clone & rename
   std::unique_ptr<cmn::node> pInstance(
      &cmn::cloneTree(pTemplate->demandSoleChild<cmn::classNode>(),false));
   dynamic_cast<cmn::classNode&>(*pInstance.get()).name
      = buildInstancePseudoName(base,args);

   // substitute args
   { typeParamedNameResolver v(dict); pInstance->acceptVisitor(v); v.finish(); }

   // attach
   pTemplate->getAncestor<cmn::fileNode>().appendChild(*pInstance.release());

   n.impled = true;
}

void classInstantiator::buildBinding(std::list<std::string>& args, cmn::genericNode& n, std::map<std::string,std::string>& dict)
{
   auto cons = n.getChildrenOf<cmn::constraintNode>();
   if(cons.size() != args.size())
      cdwTHROW("wrong number of arguments when intantiating generic %s",
         n.demandSoleChild<cmn::classNode>().name.c_str());

   auto ait = args.begin();
   for(auto cit=cons.begin();cit!=cons.end();++cit,++ait)
   {
      cdwVERBOSE("  binding '%s' = '%s'\n",(*cit)->name.c_str(),ait->c_str());
      cmn::addUnique(dict,(*cit)->name,*ait);
   }
}

std::string classInstantiator::buildInstancePseudoName(const std::string& base, const std::list<std::string>& args)
{
   // we're inject the instance in the same file as the generic, so it already has the
   // name scope.  Don't repeat it by using a FQN.
   std::string shortName;
   cmn::nameUtil::stripLast(base,shortName);

   std::stringstream stream;
   stream << shortName << "<";
   for(auto it=args.begin();it!=args.end();++it)
   {
      if(it!=args.begin())
         stream << ",";
      stream << *it;
   }
   stream << ">";

   return stream.str();
}

void genericStripper::visit(cmn::fileNode& n)
{
   cmn::autoNodeDeleteOperation o;

   // remove generic classes
   auto gs = n.getChildrenOf<cmn::genericNode>();
   for(auto it=gs.begin();it!=gs.end();++it)
   {
      (*it)->getParent()->removeChild(**it);
      delete *it;
      nRemoved++;
   }

   // remove instantiate statements
   auto is = n.getChildrenOf<cmn::instantiateNode>();
   for(auto it=is.begin();it!=is.end();++it)
   {
      (*it)->getParent()->removeChild(**it);
      delete *it;
      nRemoved++;
   }
}

} // namespace philemon
