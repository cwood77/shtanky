#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "batGen.hpp"

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
   for(auto it=m_files.begin();it!=m_files.end();++it)
      m_s.stream()
         << "bin\\out\\debug\\liam"
            << " \""
            << cmn::pathUtil::addExtension(*it,cmn::pathUtil::kExtLiamSource)
            << "\""
         << std::endl
      ;

   m_s.stream()
      << std::endl
      << "echo *******************" << std::endl
      << "echo ***   shtasm    ***" << std::endl
      << "echo *******************" << std::endl
      << "bin\\out\\debug\\shtasm \"testdata\\sht\\oscall.asm\"" << std::endl
   ;
   for(auto it=m_files.begin();it!=m_files.end();++it)
      m_s.stream()
         << "bin\\out\\debug\\shtasm"
            << " \""
            << cmn::pathUtil::addExtension(*it,"ls.asm")
            << "\""
         << std::endl
      ;

   m_s.stream()
      << std::endl
      << "echo *******************" << std::endl
      << "echo ***   shlink    ***" << std::endl
      << "echo *******************" << std::endl
      << "bin\\out\\debug\\shlink \".\\testdata\\test\\.app\""
   ;
   for(auto it=m_files.begin();it!=m_files.end();++it)
      m_s.stream()
         << " \""
         << cmn::pathUtil::addExtension(*it,"ls.asm.o")
         << "\""
      ;
   m_s.stream()
      << " \"testdata\\sht\\oscall.asm.o\""
      << std::endl
   ;
}

void batGen::visit(cmn::fileNode& n)
{
   m_files.push_back(n.fullPath);
}

} // namespace araceli
