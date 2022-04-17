#include "out.hpp"
#include "pathUtil.hpp"
#include "trace.hpp"
#include <fstream>

namespace cmn {

void outStream::updateDisk(iFileWriter& f)
{
   f.skipWriteOrDelete(m_fullPath,m_stream.str());
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
   key = cmn::pathUtil::addExt(basePath,ext);
   return _get(key);
}

iOutStream*& outBundle::_get(const std::string& key)
{
   iOutStream*& pStream = m_streams[key];
   return pStream;
}

void unconditionalWriter::skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const
{
   cdwVERBOSE("writing %s\n",fullPath.c_str());
   std::ofstream file(fullPath.c_str());
   file << newContents;
}

void stdoutFileWriter::skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const
{
   cdwVERBOSE("stdoutFileWriter -----------------\n");
   cdwVERBOSE("would have written to file %s\n",fullPath.c_str());
   cdwVERBOSE("%s",newContents.c_str());
}

} // namespace cmn
