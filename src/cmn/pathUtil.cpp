#include "pathUtil.hpp"
#include "throw.hpp"
#include "trace.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string.h>

namespace cmn {

const char *pathUtil::kExtLiamHeader      = "lh";
const char *pathUtil::kExtLiamSource      = "ls";

const char *pathUtil::kExtLir             = "lir";
const char *pathUtil::kExtLirPost         = "lir-post";
const char *pathUtil::kExtAsm             = "asm";
const char *pathUtil::kExtObj             = "o";
const char *pathUtil::kExtList            = "list";
const char *pathUtil::kExtMcList          = "mclist";
const char *pathUtil::kExtMcOdaList       = "mcODAlist";

std::string pathUtil::addExt(const std::string& path, const std::string& ext)
{
   return path + "." + ext;
}

std::string pathUtil::getExt(const std::string& path)
{
   const char *pDot = ::strrchr(path.c_str(),'.');
   if(!pDot)
      cdwTHROW("bad path format");
   return pDot+1;
}

std::string pathUtil::replaceOrAddExt(const std::string& path, const std::string& old, const std::string& ext)
{
   std::list<std::string> l;
   splitPath(path,l,false);

   auto& name = l.back();
   std::list<std::string> l2;
   splitName(name,l2);

   if(l2.back() == old)
      l2.back() = ext;
   else
      l2.push_back(ext);

   name = combineName(l2);
   l.back() = name;
   return combinePath(l);
}

std::string pathUtil::getLastPart(const std::string& path)
{
   const char *pSlash = ::strrchr(path.c_str(),'\\');
   if(!pSlash)
      cdwTHROW("bad path format");
   return pSlash+1;
}

std::string pathUtil::addPrefixToFilePart(const std::string& path, const std::string& prefix)
{
   std::list<std::string> l;
   splitPath(path,l,false);
   std::string revised = prefix + l.back();
   l.back() = revised;
   return combinePath(l);
}

std::string pathUtil::setInfix(const std::string& fullPath, const std::string& infix)
{
   std::list<std::string> l;
   splitPath(fullPath,l,true);

   auto& name = l.back();
   std::list<std::string> l2;
   splitName(name,l2);

   if(l2.size() < 2)
      cdwTHROW("insanity");
   if(l2.size() == 2 || (l2.size() == 3 && l2.begin()->empty()))
      l2.insert(--(l2.end()),"-tbd-");

   *(--(--(l2.end()))) = infix;

   name = combineName(l2);
   return combinePath(l);
}

std::string pathUtil::getInfix(const std::string& fullPath, std::string& prefix)
{
   std::list<std::string> l;
   splitPath(fullPath,l,true);

   auto& name = l.back();
   std::list<std::string> l2;
   splitName(name,l2);

   std::string infix;
   l2.erase(--l2.end()); // shave off ext
   if(l2.size() == 1)
      ; // no infix
   else if(l2.size() == 2 && l2.begin()->empty())
      ; // no infix
   else
   {
      infix = l2.back();
      l2.erase(--l2.end());
   }

   prefix = combineName(l2);
   return infix;
}

// include .\testdata\sht\cons\program.ara.lh
//    from .\testdata\test\test.ara.ls
//              translates to -> ..\sht\cons\program.ara.lh
std::string pathUtil::computeRefPath(const std::string& refer, const std::string& refee)
{
   std::list<std::string> referL, refeeL, ans;

   // convert to lists (and ignore '.')
   splitPath(refer,referL,true);
   splitPath(refee,refeeL,true);

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
   splitPath(x,l,true);
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

void pathUtil::splitPath(const std::string& path, std::list<std::string>& list, bool removeDots)
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

         if(word == ".." && list.size() && removeDots)
         {
            list.erase(--list.end());
         }
         else if(word != "." || !removeDots)
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

void pathUtil::splitName(const std::string& path, std::list<std::string>& list)
{
   const char *pStart = path.c_str();
   const char *pThumb = path.c_str();

   for(;*pThumb;pThumb++)
   {
      if(*pThumb=='.')
      {
         std::string word(pStart,pThumb-pStart);
         pStart = pThumb + 1;
         pThumb = pStart;
         list.push_back(word);
      }
   }

   std::string word(pStart,pThumb-pStart);
   list.push_back(word);
}

std::string pathUtil::combineName(const std::list<std::string>& list)
{
   std::stringstream stream;
   for(auto it=list.begin();it!=list.end();++it)
   {
      if(it!=list.begin())
         stream << ".";
      stream << *it;
   }
   return stream.str();
}

void sourceFileGroup::add(const std::string& filename)
{
   std::string prefix;
   std::string infix = pathUtil::getInfix(filename,prefix);
   cdwDEBUG("split '%s' => [%s][%s]\n",
      filename.c_str(),
      prefix.c_str(),
      infix.c_str());
   if(infix > m_lastInfix)
   {
      cdwDEBUG("discarding as too new\n");
      return;
   }

   std::string& current = m_prefixToInfix[prefix];
   if(infix > current)
   {
      cdwDEBUG("new record ('%s' > '%s'\n",infix.c_str(),current.c_str());
      current = infix;
   }
}

void sourceFileGroup::except(const std::string& prefix)
{
   m_prefixToInfix.erase(prefix);
}

std::list<std::string> sourceFileGroup::getLatest()
{
   std::list<std::string> rval;

   for(auto it=m_prefixToInfix.begin();it!=m_prefixToInfix.end();++it)
   {
      std::string x = pathUtil::addExt(it->first,m_ext);
      if(!it->second.empty())
         x = pathUtil::setInfix(x,it->second);
      rval.push_back(x);
   }

   return rval;
}

} // namespace cmn
