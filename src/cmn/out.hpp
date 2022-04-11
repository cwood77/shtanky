#pragma once
#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace cmn {

class iFileWriter {
public:
   virtual void skipWriteOrDelete(const std::string& fullPath, const std::string& newContents) const = 0;
};

class iOutStream {
public:
   virtual ~iOutStream() {}

   virtual void updateDisk(iFileWriter& f) = 0;
};

class outStream : public iOutStream {
public:
   explicit outStream(const std::string& fullPath) : m_fullPath(fullPath) {}

   std::ostream& stream() { return m_stream; }

   virtual void updateDisk(iFileWriter& f);

private:
   std::string m_fullPath;
   std::stringstream m_stream;
};

class columnedOutStream : public iOutStream {
public:
   explicit columnedOutStream(const std::string& fullPath)
   : m_fullPath(fullPath), m_width(3) {}
   ~columnedOutStream() {}

   void setWidth(size_t w) { m_width = w; }

   std::vector<std::stringstream*>& appendLine();

   virtual void updateDisk(iFileWriter& f);

private:
   std::string m_fullPath;
   size_t m_width;
   std::list<std::vector<std::stringstream*> > m_lines;
};

class outBundle {
public:
   outBundle() : m_pWriter(NULL) {}
   ~outBundle();

   template<class T>
   T& get(const std::string& key)
   {
      iOutStream*& pStream = _get(key);
      if(!pStream)
         pStream = new T(key);
      return dynamic_cast<T&>(*pStream);
   }

   template<class T>
   T& get(const std::string& basePath, const std::string& ext)
   {
      std::string key;
      iOutStream*& pStream = _get(basePath,ext,key);
      if(!pStream)
         pStream = new T(key);
      return dynamic_cast<T&>(*pStream);
   }

#if 0
   // ephemeral files are always newly-created; if one already exists, a new one
   // with a decorated name is created
   outStream& getEphemeral(const std::string& basePath, const std::string& ext);
#endif

   void setAutoUpdate(iFileWriter& f) { m_pWriter = &f; }
   void updateDisk(iFileWriter& f);

private:
   iOutStream*& _get(const std::string& basePath, const std::string& ext, std::string& key);
   iOutStream*& _get(const std::string& key);

   std::map<std::string,iOutStream*> m_streams;
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
