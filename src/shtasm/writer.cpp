#include "../cmn/fmt.hpp"
#include "writer.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace shtasm {

binFileWriter::binFileWriter(const std::string& filePath)
: m_pFile(NULL)
{
   m_pFile = ::fopen(filePath.c_str(),"wb");
   if(!m_pFile)
      throw std::runtime_error(cmn::fmt("can't open file '%s' for writing",filePath.c_str()));
}

binFileWriter::~binFileWriter()
{
   ::fclose(m_pFile);
}

void binFileWriter::write(const void *p, size_t n)
{
   ::fwrite(p,1,n,m_pFile);
}

int binFileWriter::tell()
{
   return ::ftell(m_pFile);
}

void binFileWriter::seek(int o)
{
   ::fseek(m_pFile,o,SEEK_SET);
}

size_t retailObjWriter::reservePatch(const std::string& reason, size_t n)
{
   size_t k = m_nextKey++;
   m_patches[k].first = n;
   m_patches[k].second = m_pS->tell();
   return k;
}

void retailObjWriter::fillPatch(size_t k, const void *p)
{
   auto& pi = m_patches[k];
   int here = m_pS->tell();
   m_pS->seek(pi.second);
   m_pS->write(p,pi.first);
   m_pS->seek(here);
}

void retailObjWriter::write(size_t lineNum, const std::string& reason, const void *p, size_t n)
{
   m_pS->write(p,n);
}

size_t listingObjWriter::reservePatch(const std::string& reason, size_t n)
{
   size_t id = m_patchSizes.size();

   std::stringstream stream;
   stream << reason << "(patch;size=" << n << ",id=" << id << ")";
   m_patchSizes[id] = n;

   std::string copy = stream.str();
   m_pS->write(copy.c_str(),copy.length());

   return id;
}

void listingObjWriter::fillPatch(size_t k, const void *p)
{
   std::stringstream stream;
   stream << "FILLPATCH(id=" << k << ",";
   fmtData(stream,p,m_patchSizes[k]);

   std::string copy = stream.str();
   m_pS->write(copy.c_str(),copy.length());
}

void listingObjWriter::write(size_t lineNum, const std::string& reason, const void *p, size_t n)
{
   std::stringstream stream;
   stream << reason << "(";
   fmtData(stream,p,n);
   stream << ")";

   std::string copy = stream.str();
   m_pS->write(copy.c_str(),copy.length());
}

void listingObjWriter::nextPart()
{
   std::stringstream stream;
   stream << std::endl;

   std::string copy = stream.str();
   m_pS->write(copy.c_str(),copy.length());
}

void listingObjWriter::fmtData(std::ostream& o, const void *p, size_t n)
{
   const unsigned char *_p = reinterpret_cast<const unsigned char *>(p);
   for(size_t i=0;i<n;i++)
   {
      const char *pSep = "";
      if(i > 0)
      {
         if(i % 4 == 0)
            pSep = ",";
         else
            pSep = "-";
      }
      o << pSep
         << std::hex << std::setfill('0') << std::setw(2)
         << (unsigned int)_p[i];
   }
}

compositeObjWriter::~compositeObjWriter()
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      delete *it;
}

size_t compositeObjWriter::reservePatch(const std::string& reason, size_t n)
{
   size_t k = m_nextKey++;
   auto& p = m_patches[k];

   for(auto it=m_o.begin();it!=m_o.end();++it)
      p[*it] = (*it)->reservePatch(reason,n);

   return k;
}

void compositeObjWriter::fillPatch(size_t k, const void *_p)
{
   auto& p = m_patches[k];

   for(auto it=m_o.begin();it!=m_o.end();++it)
      (*it)->fillPatch(p[*it],_p);
}

void compositeObjWriter::write(size_t lineNum, const std::string& reason, const void *p, size_t n)
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      (*it)->write(lineNum,reason,p,n);
}

void compositeObjWriter::nextPart()
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      (*it)->nextPart();
}

} // namespace shtasm
