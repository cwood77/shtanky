#include "loader.hpp"
#include "symbolTable.hpp"
#include <string.h>

namespace araceli {

unloadedScopeFinder::unloadedScopeFinder(const std::string& missingRef)
: m_missingRef(missingRef)
{
   // if the ref is FQ, shave off the leading .
   // so I can treat FQ and rel names identically
   if(m_missingRef.c_str()[0] == '.')
      m_missingRef = m_missingRef.c_str()+1;
}

bool unloadedScopeFinder::any()
{
   return m_candidates.size();
}

cmn::scopeNode& unloadedScopeFinder::mostLikely()
{
   return *((--(m_candidates.end()))->second);
}

void unloadedScopeFinder::visit(cmn::scopeNode& n)
{
   if(!n.loaded)
   {
      std::string fqn = cmn::fullyQualifiedName::build(n);

      if(
         ((fqn.length()-1) <= m_missingRef.length()) &&
         ::strncmp(fqn.c_str()+1,m_missingRef.c_str(),fqn.length()-1)==0
        )
      {
         m_candidates[fqn] = &n;
      }
      else
      {
         ::printf("   scope %s not a candidate\n",n.path.c_str());
         ::printf("      %s vs %s\n",fqn.c_str(),m_missingRef.c_str());
      }
   }
   else
      ::printf("   scope %s already loaded\n",n.path.c_str());

   cmn::hNodeVisitor::visit(n);
}

bool nodeLinker::loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable)
{
   cmn::scopeNode *pToLoad = NULL;
   for(auto it=sTable.unresolved.begin();it!=sTable.unresolved.end();++it)
   {
      auto refToFind = (*it)->ref;
      unloadedScopeFinder f(refToFind);
      root.acceptVisitor(f);
      if(f.any())
      {
         ::printf("trying to find symbol %s\n",refToFind.c_str());
         pToLoad = &f.mostLikely();
         break;
      }
   }

   if(pToLoad)
   {
      ::printf("loading %s and trying again\n",pToLoad->path.c_str());
      loader::loadFolder(*pToLoad);
      { cmn::diagVisitor v; root.acceptVisitor(v); }
   }

   return pToLoad;
}

} // namespace araceli
