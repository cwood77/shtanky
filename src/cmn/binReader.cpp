#include "../cmn/fmt.hpp"
#include "binReader.hpp"
#include "throw.hpp"
#include <cstring>
#include <memory>

namespace cmn {

binFileReader::binFileReader(const std::string& filePath)
: m_pFile(NULL)
{
   m_pFile = ::fopen(filePath.c_str(),"rb");
   if(!m_pFile)
      cdwTHROW(cmn::fmt("can't open file '%s' for reading",filePath.c_str()));
}

binFileReader::~binFileReader()
{
   ::fclose(m_pFile);
}

void binFileReader::read(void *p, size_t n)
{
   ::fread(p,1,n,m_pFile);
}

std::string binFileReader::readString()
{
   auto l = iObjReader::read<size_t>();
   std::unique_ptr<char[]> pBuffer(new char[l]);
   read(pBuffer.get(),l);
   return std::string(pBuffer.release(),l);
}

void binFileReader::readThumbprint(const char *pExpected)
{
   size_t l = ::strlen(pExpected);
   char thumbprint[l+1];
   ::memset(thumbprint,0,l+1);
   read(thumbprint,l);
   if(std::string(pExpected) != thumbprint)
      cdwTHROW("bad file format - failed thumprint");
}

} // namespace cmn
