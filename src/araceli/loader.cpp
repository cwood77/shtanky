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

cmn::timedGlobal<std::string> gLastSupportedInfix;

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

   cmn::sourceFileGroup grp(*gLastSupportedInfix,"ara");

   // discover files
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
      else if(cmn::pathUtil::getExt(fullPath) == "ara")
      {
         grp.add(fullPath);
#if 0
         if(cmn::pathUtil::getLastPart(fullPath) == ".target.ara")
            cdwVERBOSE("skipping load of %s\n",fullPath.c_str());
         else
            loadFile(s,fullPath);
#endif
      }

   } while(::FindNextFile(hFind,&fData));
   ::FindClose(hFind);

   // load files
   grp.except(".target");
   {
      auto l = grp.getLatest();
      for(auto it=l.begin();it!=l.end();++it)
         loadFile(s,s.path + "\\" + *it);
   }

   // mark loaded
   s.loaded = true;
}

void loader::loadFile(cmn::scopeNode& s, const std::string& fullPath)
{
   cdwVERBOSE("loading file %s\n",fullPath.c_str());
   std::string contents;
   cmn::pathUtil::loadFileContents(fullPath,contents);
   lexor l(contents.c_str());
   l.setFileName(fullPath);
   cmn::commonParser p(l);
   auto file = p.parseFile();
   file->fullPath = fullPath;
   s.appendChild(*file.release());
}

} // namespace araceli
