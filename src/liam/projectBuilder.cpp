#include "../cmn/commonParser.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "lexor.hpp"
#include "projectBuilder.hpp"

namespace liam {

void fileRefFinder::visit(cmn::fileRefNode& n)
{
   cdwVERBOSE("found reference to file %s\n",n.ref.c_str());
   m_filesToLoad.insert(cmn::pathUtil::absolutize(m_pFile->fullPath,n.ref));
}

void projectBuilder::build(cmn::liamProjectNode& p)
{
   projectBuilder self(p);
   self.m_filesToLoad.insert(p.sourceFullPath);
   self.load();
}

void projectBuilder::load()
{
   while(true)
   {
      std::string fileToLoad = getNextFileToLoad();
      if(fileToLoad.empty())
         return;

      loadFile(fileToLoad);
   }
}

std::string projectBuilder::getNextFileToLoad()
{
   while(m_filesToLoad.size())
   {
      auto file = *m_filesToLoad.begin();
      m_filesToLoad.erase(m_filesToLoad.begin());

      if(m_loadedFiles.find(file)==m_loadedFiles.end())
      {
         m_loadedFiles.insert(file);
         return file;
      }
   }

   return ""; // nothing to load
}

void projectBuilder::loadFile(const std::string& fileToLoad)
{
   cdwVERBOSE("loading file %s\n",fileToLoad.c_str());
   std::string contents;
   cmn::pathUtil::loadFileContents(fileToLoad,contents);
   lexor l(contents.c_str());
   cmn::commonParser p(l);
   auto file = p.parseFile();
   file->fullPath = fileToLoad;
   m_root.appendChild(*file.release());
   gatherMoreFilesToLoad(*dynamic_cast<cmn::fileNode*>(m_root.lastChild()));
}

void projectBuilder::gatherMoreFilesToLoad(cmn::fileNode& f)
{
   fileRefFinder v(m_filesToLoad);
   f.acceptVisitor(v);
}

} // namespace liam
