#include "out.hpp"
#include "pathUtil.hpp"
#include <fstream>

namespace cmn {

void outStream::updateDisk(iFileWriter& f)
{
   f.skipWriteOrDelete(m_fullPath,m_stream.str());
}

std::vector<std::stringstream*>& columnedOutStream::appendLine()
{
   std::vector<std::stringstream*> newLine;
   for(size_t i=0;i<m_width;i++)
      newLine.push_back(new std::stringstream());
   m_lines.push_back(newLine);
   return *(--(m_lines.end()));
}

void columnedOutStream::updateDisk(iFileWriter& f)
{
   std::stringstream s;
   for(auto it=m_lines.begin();it!=m_lines.end();++it)
   {
      for(auto jit=it->begin();jit!=it->end();++jit)
      {
         s << (*jit)->str() << " ";
      }
      s << std::endl;
   }
   f.skipWriteOrDelete(m_fullPath,s.str());
}

outBundle::~outBundle()
{
   if(m_pWriter)
      updateDisk(*m_pWriter);

   for(auto it=m_streams.begin();it!=m_streams.end();++it)
      delete it->second;
}

void outBundle::updateDisk(iFileWriter& f)
{
   for(auto it=m_streams.begin();it!=m_streams.end();++it)
      it->second->updateDisk(f);
}

iOutStream*& outBundle::_get(const std::string& basePath, const std::string& ext, std::string& key)
{
   key = cmn::pathUtil::addExtension(basePath,ext);
   iOutStream*& pStream = m_streams[key];
   return pStream;
}

void fileWriter::skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const
{
   ::printf("writing %s\n",fullPath.c_str());
   std::ofstream file(fullPath.c_str());
   file << newContents;
}

void testFileWriter::skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const
{
   ::printf("testFileWriter -----------------\n");
   ::printf("would have written to file %s\n",fullPath.c_str());
   ::printf("%s",newContents.c_str());
}

} // namespace cmn
