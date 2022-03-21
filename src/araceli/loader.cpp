#define WIN32_LEAN_AND_MEAN
#include "../cmn/pathUtil.hpp"
#include "ast.hpp"
#include "lexor.hpp"
#include "loader.hpp"
#include "parser.hpp"
#include <stdio.h>
#include <windows.h>

namespace araceli {

void loader::loadFolder(scopeNode& s)
{
   if(s.loaded)
      return;

   // load subfolders
   for(auto it=s.getChildren().begin();it!=s.getChildren().end();++it)
   {
      scopeNode *pSubScope = dynamic_cast<scopeNode*>(*it);
      if(pSubScope)
         loadFolder(*pSubScope);
   }

   // load files
   std::string pattern = s.path + "\\*";
   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFile(pattern.c_str(),&fData);
   do {
      if(std::string(".") == fData.cFileName)
         continue;
      else if(std::string("..") == fData.cFileName)
         continue;

      std::string fullPath = s.path + "\\" + fData.cFileName;

      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         ;
      else
      {
         ::printf("loading file %s\n",fullPath.c_str());
         std::string contents;
         cmn::pathUtil::loadFileContents(fullPath,contents);
         lexor l(contents.c_str());
         parser p(l);
         auto file = p.parseFile();
         s.appendChild(*file.release());
      }

   } while(::FindNextFile(hFind,&fData));
   ::FindClose(hFind);

   // mark loaded
   s.loaded = true;
}

} // namespace araceli
