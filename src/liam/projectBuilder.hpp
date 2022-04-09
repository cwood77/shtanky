#pragma once
#include <set>
#include "../cmn/ast.hpp"

namespace cmn { class liamProjectNode; }

namespace liam {

class fileRefFinder : public cmn::liamVisitor<> {
public:
   explicit fileRefFinder(std::set<std::string>& filesToLoad)
   : m_pFile(NULL), m_filesToLoad(filesToLoad) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n) { m_pFile = &n; hNodeVisitor::visit(n); }
   virtual void visit(cmn::fileRefNode& n);

private:
   cmn::fileNode *m_pFile;
   std::set<std::string>& m_filesToLoad;
};

class projectBuilder {
public:
   static void build(cmn::liamProjectNode& p);

private:
   projectBuilder(cmn::liamProjectNode& root) : m_root(root) {}

   void load();

   std::string getNextFileToLoad();
   void loadFile(const std::string& fileToLoad);
   void gatherMoreFilesToLoad(cmn::fileNode& f);

   //bool loadFileIf(const std::string& fullPath);

   cmn::liamProjectNode& m_root;
   std::set<std::string> m_loadedFiles;
   std::set<std::string> m_filesToLoad;
};

} // namespace liam
