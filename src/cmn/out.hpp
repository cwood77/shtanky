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
   explicit outStream(const std::string& fullPath) : m_fullPath(fullPath), m_indent(0) {}

   std::ostream& stream() { return m_stream; }

   virtual void updateDisk(iFileWriter& f);

   void incIndent() { m_indent += 3; }
   void decIndent() { m_indent -= 3; }
   size_t getIndent() const { return m_indent; }

private:
   std::string m_fullPath;
   std::stringstream m_stream;
   size_t m_indent;
};

class indent {
public:
   explicit indent(const outStream& s) : m_stream(s) {}

   std::string buildIndent() const { return std::string(m_stream.getIndent(),' '); }

private:
   const outStream& m_stream;
};

inline std::ostream& operator<<(std::ostream& o, const indent& i)
{ o << i.buildIndent(); return o; }

class autoIndent {
public:
   autoIndent(outStream& s) : m_stream(s) { m_stream.incIndent(); }
   ~autoIndent() { m_stream.decIndent(); }

private:
   outStream& m_stream;
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
