#include "pathUtil.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>

namespace cmn {

std::string pathUtil::getExtension(const std::string& path)
{
   const char *pDot = ::strrchr(path.c_str(),'.');
   if(!pDot)
      throw std::runtime_error("bad path format");
   return pDot+1;
}

std::string pathUtil::getLastPart(const std::string& path)
{
   const char *pSlash = ::strrchr(path.c_str(),'\\');
   if(!pSlash)
      throw std::runtime_error("bad path format");
   return pSlash+1;
}

void pathUtil::loadFileContents(const std::string& path, std::string& contents)
{
   std::stringstream stream;

   std::ifstream src(path.c_str());
   while(src.good())
   {
      std::string line;
      std::getline(src,line);
      stream << line << std::endl;
   }

   contents = stream.str();
}

} // namespace cmn
