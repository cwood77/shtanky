#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "batGen.hpp"
#include "iTarget.hpp"

namespace araceli {

void batGen::visit(cmn::araceliProjectNode& n)
{
   m_s.stream()
      << "@echo off" << std::endl
   ;

   hNodeVisitor::visit(n);

   m_s.stream()
      << std::endl
      << "echo *****************" << std::endl
      << "echo ***   liam    ***" << std::endl
      << "echo *****************" << std::endl
   ;
   {
      std::list<std::string> f = m_files;
      adjustFiles(iTarget::kLiamPhase,cmn::pathUtil::kExtLiamSource,f);
      for(auto it=f.begin();it!=f.end();++it)
         m_s.stream()
            << "bin\\out\\debug\\liam"
               << " \""
               << *it
               << "\""
            << std::endl
         ;
   }

   m_s.stream()
      << std::endl
      << "echo *******************" << std::endl
      << "echo ***   shtasm    ***" << std::endl
      << "echo *******************" << std::endl
   ;
   {
      std::list<std::string> f = m_files;
      adjustFiles(iTarget::kShtasmPhase,"ls.asm",f);
      for(auto it=f.begin();it!=f.end();++it)
         m_s.stream()
            << "bin\\out\\debug\\shtasm"
               << " \""
               << *it
               << "\""
            << std::endl
         ;
   }

   m_s.stream()
      << std::endl
      << "echo *******************" << std::endl
      << "echo ***   shlink    ***" << std::endl
      << "echo *******************" << std::endl
      << "bin\\out\\debug\\shlink \".\\testdata\\test\\.app\""
   ;
   {
      std::list<std::string> f = m_files;
      adjustFiles(iTarget::kShlinkPhase,"ls.asm.o",f);
      for(auto it=f.begin();it!=f.end();++it)
         m_s.stream()
            << " \""
            << *it
            << "\""
         ;
   }

   m_s.stream() << std::endl;
}

void batGen::visit(cmn::fileNode& n)
{
   m_files.push_back(n.fullPath);
}

void batGen::adjustFiles(iTarget::phase p, const std::string ext, std::list<std::string>& l)
{
   std::list<std::string> result;

   for(auto it=l.begin();it!=l.end();++it)
      result.push_back(cmn::pathUtil::addExt(*it,ext));

   m_tgt.adjustFiles(p,result);

   l = result;
}

} // namespace araceli
