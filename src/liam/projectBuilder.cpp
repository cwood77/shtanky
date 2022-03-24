#include "../cmn/ast.hpp"
#include "../cmn/commonParser.hpp"
#include "../cmn/pathUtil.hpp"
#include "lexor.hpp"
#include "projectBuilder.hpp"
#include <stdio.h>

namespace liam {

void projectBuilder::build(cmn::liamProjectNode& p)
{
   loadFileIf(p,p.sourceFullPath);
}

bool projectBuilder::loadFileIf(cmn::liamProjectNode& prj, const std::string& fullPath)
{
   ::printf("loading file %s\n",fullPath.c_str());
   std::string contents;
   cmn::pathUtil::loadFileContents(fullPath,contents);
   lexor l(contents.c_str());
   cmn::commonParser p(l);
   auto file = p.parseFile();
   file->fullPath = fullPath;
   prj.appendChild(*file.release());

   return true;
}

} // namespace liam
