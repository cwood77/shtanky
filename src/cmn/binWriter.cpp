#include "binWriter.hpp"
#include "fmt.hpp"
#include "pathUtil.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace cmn {

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

binMemoryWriter::binMemoryWriter(std::unique_ptr<unsigned char[]>& block)
: m_block(block)
, m_s(0)
{
   m_alloc.resize(1024);
}

binMemoryWriter::~binMemoryWriter()
{
   m_block.reset(new unsigned char [m_s]);
   ::memcpy(m_block.get(),&m_alloc[0],m_s);
}

void binMemoryWriter::write(const void *p, size_t n)
{
   size_t left = m_alloc.size() - m_s;
   if(left >= n)
   {
      ::memcpy(&m_alloc[0] + m_s,p,n);
      m_s += n;
   }
   else
   {
      m_alloc.resize(m_alloc.size() * 2);
      write(p,n);
   }
}

void iObjWriter::writeCommentLine(const std::string& comment)
{
   std::stringstream buffer;
   buffer << "// " << comment << std::endl;
   write(buffer.str(),NULL,0);
}

void iObjWriter::writeCommentLine(unsigned long line, const std::string& comment)
{
   std::stringstream buffer;
   buffer << "// " << line << ": " << comment << std::endl;
   write(buffer.str(),NULL,0);
}

void retailObjWriter::write(const std::string& reason, const void *p, size_t n)
{
   m_pS->write(p,n);
}

void listingObjWriter::write(const std::string& reason, const void *p, size_t n)
{
   std::stringstream stream;
   stream << reason;
   if(p && n)
   {
      stream << "(";
      fmtData(stream,p,n);
      stream << ")";
   }

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

void odaObjWriter::write(const std::string& reason, const void *p, size_t n)
{
   std::stringstream stream;
   fmtData(stream,p,n);

   std::string copy = stream.str();
   m_pS->write(copy.c_str(),copy.length());
}

void odaObjWriter::fmtData(std::ostream& o, const void *p, size_t n)
{
   const unsigned char *_p = reinterpret_cast<const unsigned char *>(p);
   for(size_t i=0;i<n;i++)
   {
      o << " "
         << std::hex << std::setfill('0') << std::setw(2)
         << (unsigned int)_p[i];
   }
}

compositeObjWriter::~compositeObjWriter()
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      delete *it;
}

void compositeObjWriter::write(const std::string& reason, const void *p, size_t n)
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      (*it)->write(reason,p,n);

   m_offset += n;
}

void compositeObjWriter::nextPart()
{
   for(auto it=m_o.begin();it!=m_o.end();++it)
      (*it)->nextPart();
}

// a frequent setup used in shtasm and shlink
void compositeObjWriter::sinkNewFileWithListing(const std::string& path)
{
   sinkNewFile<retailObjWriter>(path);
   sinkNewFile<listingObjWriter>(pathUtil::addExt(path,pathUtil::kExtList));
}

} // namespace cmn
