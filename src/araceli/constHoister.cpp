#include "constHoister.hpp"

namespace araceli {

void constHoister::visit(cmn::fileNode& n)
{
   m_pFile = &n;
   m_next = 0;
   hNodeVisitor::visit(n);
}

// const will have the type and literal as children
// literal will be replaced with a varRef
// const is dumped on the file
void constHoister::visit(cmn::stringLiteralNode& n)
{
   // create
   std::unique_ptr<cmn::constNode> pConst(new cmn::constNode());
   std::unique_ptr<cmn::strTypeNode> pType(new cmn::strTypeNode());
   std::unique_ptr<cmn::varRefNode> pRef(new cmn::varRefNode());

   // configure
#if 0
   {
      std::stringstream name;
      name << ".const" << m_next++;
      pConst->name = name.str();
   }
#endif
   pConst->name = chooseConstName(n);
   pRef->pSrc.ref = cmn::fullyQualifiedName::build(*m_pFile,pConst->name);

   // chain
   pConst->appendChild(*pType.release());
   n.getParent()->replaceChild(n,*pRef.release());
   pConst->appendChild(n);
   m_pFile->appendChild(*pConst.release());
}

std::string constHoister::chooseConstName(cmn::stringLiteralNode& n)
{
   std::stringstream name;
   name << "._strLit_";

   const size_t capDelta = ('a' - 'A');
   bool truncated = false;
   bool wordBoundary = false;
   size_t len = n.value.length();
   if(len > 15)
   {
      truncated = true;
      len = 15;
   }
   for(size_t i=0;i<len;i++)
   {
      bool digit = false;
      char c = n.value.c_str()[i];
      if('a' <= c && c <= 'z')
         ; // lowercase
      else if('A' <= c && c <= 'Z')
         c += capDelta; // uppercase
      else if('0' <= c && c <= '9')
         digit = true; // uppercase
      else
         c = 0; // not ok

      if(c == 0 || digit)
         wordBoundary = true;
      else if(wordBoundary)
      {
         c -= capDelta;
         wordBoundary = false;
      }

      if(c)
         name << std::string(1,c);
   }
   if(truncated)
      name << "_etc";

   // uniqueness
   name << "_" << m_next++;
   return name.str();
}

} // namespace araceli
