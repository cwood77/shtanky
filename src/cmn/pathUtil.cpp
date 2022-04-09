#include "pathUtil.hpp"
#include "trace.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>

namespace cmn {

const char *pathUtil::kExtLiamHeader = "lh";
const char *pathUtil::kExtLiamSource = "ls";

std::string pathUtil::addExtension(const std::string& path, const std::string& ext)
{
   return path + "." + ext;
}

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

// include .\testdata\sht\cons\program.ara.lh
//    from .\testdata\test\test.ara.ls
//              translates to -> ..\sht\cons\program.ara.lh
std::string pathUtil::computeRefPath(const std::string& refer, const std::string& refee)
{
   std::list<std::string> referL, refeeL, ans;

   // convert to lists (and ignore '.')
   splitPath(refer,referL);
   splitPath(refee,refeeL);

   // remove common prefixes off both
   while(referL.size() > 1 && refeeL.size() > 1 && *referL.begin() == *refeeL.begin())
   {
      referL.pop_front();
      refeeL.pop_front();
   }

   // for each unique prefix on refer push_back(..)
   // (ignore the lat word on refer)
   for(size_t i=1;i<referL.size();i++)
         ans.push_back("..");

   // finally append refeee
   ans.insert(ans.end(),refeeL.begin(),refeeL.end());

   return combinePath(ans);
}

std::string pathUtil::absolutize(const std::string& refer, const std::string& refee)
{
   std::string x = refer + "\\..\\" + refee;
   std::list<std::string> l;
   splitPath(x,l);
   return combinePath(l);
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

void pathUtil::splitPath(const std::string& path, std::list<std::string>& list)
{
   const char *pStart = path.c_str();
   const char *pThumb = path.c_str();

   for(;*pThumb;pThumb++)
   {
      if(*pThumb=='\\' || *pThumb=='/')
      {
         std::string word(pStart,pThumb-pStart);
         pStart = pThumb + 1;
         pThumb = pStart;

         if(word == ".." && list.size())
         {
            list.erase(--list.end());
         }
         else if(word != ".")
         {
            list.push_back(word);
         }
      }
   }

   std::string word(pStart,pThumb-pStart);
   list.push_back(word);
}

std::string pathUtil::combinePath(const std::list<std::string>& list)
{
   std::stringstream stream;
   for(auto it=list.begin();it!=list.end();++it)
   {
      if(it!=list.begin())
         stream << "\\";
      stream << *it;
   }
   return stream.str();
}

} // namespace cmn
