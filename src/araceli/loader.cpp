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

cmn::timedGlobal<loaderPrefs> gLoaderPrefs;

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
      else if(cmn::pathUtil::getExt(fullPath) == gLoaderPrefs->ext)
      {
         if(cmn::pathUtil::getLastPart(fullPath) == gLoaderPrefs->ignoredFileName)
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
   l.setFileName(fullPath);
   cmn::commonParser p(l);
   auto file = p.parseFile();
   file->fullPath = fullPath;
   s.appendChild(*file.release());
}

void loader::findScopeAndLoadFile(cmn::araceliProjectNode& s, const std::string& fullPath)
{
   std::string fileName = cmn::pathUtil::getLastPart(fullPath);
   std::string folderName(fullPath.c_str(),fullPath.length() - fileName.length() - 1);
   cdwDEBUG("searching for scope '%s'\n",folderName.c_str());

   auto& scope = findScope(s,folderName);
   loadFile(scope,fullPath);
}

bool startsWith(const std::string& thing, const std::string& prefix)
{
   if(prefix.length() >= thing.length()) return false;

   return ::strncmp(prefix.c_str(),thing.c_str(),prefix.length()) == 0;
}

cmn::scopeNode& loader::findScope(cmn::node& n, const std::string& folder)
{
   auto scopes = n.getChildrenOf<cmn::scopeNode>();
   for(auto it=scopes.begin();it!=scopes.end();++it)
   {
      cdwDEBUG("checking scope '%s'\n",(*it)->path.c_str());

      if((*it)->path == folder)
         return **it;

      if(startsWith(folder,(*it)->path))
      {
         cdwDEBUG("descend\n");
         return findScope(**it,folder);
      }
   }

   cdwTHROW("can't find scope for %s\n",folder.c_str());
}

} // namespace araceli
