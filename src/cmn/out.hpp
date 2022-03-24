#pragma once
#include <map>
#include <ostream>
#include <sstream>
#include <string>

namespace cmn {

class iFileWriter {
public:
   virtual void skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const = 0;
};

class outStream {
public:
   explicit outStream(const std::string& fullPath) : m_fullPath(fullPath) {}

   std::ostream& stream() { return m_stream; }

   void updateDisk(iFileWriter& f);

private:
   std::string m_fullPath;
   std::stringstream m_stream;
};

class outBundle {
public:
   outBundle() : m_pWriter(NULL) {}
   ~outBundle();

   outStream& get(const std::string& basePath, const std::string& ext);
#if 0
   // ephemeral files are always newly-created; if one already exists, a new one
   // with a decorated name is created
   outStream& getEphemeral(const std::string& basePath, const std::string& ext);
#endif

   void setAutoUpdate(iFileWriter& f) { m_pWriter = &f; }
   void updateDisk(iFileWriter& f);

private:
   std::map<std::string,outStream*> m_streams;
   iFileWriter *m_pWriter;
};

class fileWriter : public iFileWriter {
public:
   virtual void skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const;
};

class testFileWriter : public iFileWriter {
public:
   virtual void skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const;
};

} // namespace cmn
