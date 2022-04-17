#define WIN32_LEAN_AND_MEAN
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "loader.hpp"
#include "projectBuilder.hpp"
#include <windows.h>

namespace araceli {

std::unique_ptr<cmn::araceliProjectNode> projectBuilder::create(const std::string& targetType)
{
   std::unique_ptr<cmn::araceliProjectNode> pNode(new cmn::araceliProjectNode());
   pNode->targetType = targetType;
   return pNode;
}

void projectBuilder::addScope(cmn::node& p, const std::string& path, bool inProject)
{
   cdwVERBOSE("adding scope %s\n",path.c_str());

   // add a scope node for this folder
   std::unique_ptr<cmn::scopeNode> pNode(new cmn::scopeNode());
   pNode->path = path;
   pNode->scopeName = cmn::pathUtil::getLastPart(path);
   pNode->inProject = inProject;
   cmn::scopeNode& myScope = *pNode.get();
   p.appendChild(*pNode.release());

   // check for subfolders
   std::string pattern = path + "\\*";
   WIN32_FIND_DATA fData;
   HANDLE hFind = ::FindFirstFile(pattern.c_str(),&fData);
   do {
      if(std::string(".") == fData.cFileName)
         continue;
      else if(std::string("..") == fData.cFileName)
         continue;

      std::string fullPath = path + "\\" + fData.cFileName;

      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         addScope(myScope,fullPath,inProject);
   } while(::FindNextFile(hFind,&fData));
   ::FindClose(hFind);

   // load project source immediately
   if(inProject)
      loader::loadFolder(myScope);
}

} // namespace araceli
