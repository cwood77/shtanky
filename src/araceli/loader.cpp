#define WIN32_LEAN_AND_MEAN
#include "../cmn/ast.hpp"
#include "../cmn/commonParser.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "lexor.hpp"
#include "loader.hpp"
#include <stdio.h>
#include <windows.h>

namespace araceli {

void loader::loadFolder(cmn::scopeNode& s)
{
   if(s.loaded)
      return;

   // load subfolders
   for(auto it=s.getChildren().begin();it!=s.getChildren().end();++it)
   {
      cmn::scopeNode *pSubScope = dynamic_cast<cmn::scopeNode*>(*it);
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
      else if(cmn::pathUtil::getExtension(fullPath) == "ara")
      {
         if(cmn::pathUtil::getLastPart(fullPath) == ".target.ara")
            cdwVERBOSE("skipping load of %s\n",fullPath.c_str());
         else
            loadFile(s,fullPath);
      }

   } while(::FindNextFile(hFind,&fData));
   ::FindClose(hFind);

   // mark loaded
   s.loaded = true;
}

void loader::loadFile(cmn::scopeNode& s, const std::string& fullPath)
{
   cdwVERBOSE("loading file %s\n",fullPath.c_str());
   std::string contents;
   cmn::pathUtil::loadFileContents(fullPath,contents);
   lexor l(contents.c_str());
   cmn::commonParser p(l);
   auto file = p.parseFile();
   file->fullPath = fullPath;
   s.appendChild(*file.release());
}

} // namespace araceli
