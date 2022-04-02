#pragma once
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>

namespace shtasm {

class iObjWriterSink2 {
public:
   virtual void write(const void *p, size_t n) = 0;
   virtual int tell() = 0;
   // no seek cuz this is a memory stream
};

// remove reserve/fill patch and just have ftell
// linker will patch later differently

class iObjWriterSink {
public:
   virtual ~iObjWriterSink() {}
   virtual void write(const void *p, size_t n) = 0;
   virtual int tell() = 0;
   virtual void seek(int o) = 0;
};

class binFileWriter : public iObjWriterSink {
public:
   explicit binFileWriter(const std::string& filePath);
   ~binFileWriter();
   virtual void write(const void *p, size_t n);
   virtual int tell();
   virtual void seek(int o);

private:
   FILE *m_pFile;
};

class iObjWriter {
public:
   virtual ~iObjWriter() {}

   virtual size_t reservePatch(const std::string& reason, size_t n) = 0;
   virtual void fillPatch(size_t k, const void *p) = 0;

   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n) = 0;

   virtual void nextPart() = 0;
};

class retailObjWriter : public iObjWriter {
public:
   explicit retailObjWriter(iObjWriterSink& s) : m_pS(&s), m_nextKey(0) {}
   virtual size_t reservePatch(const std::string& reason, size_t n);
   virtual void fillPatch(size_t k, const void *p);
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart() {}

private:
   std::unique_ptr<iObjWriterSink> m_pS;
   size_t m_nextKey;
   std::map<size_t,std::pair<size_t,int> > m_patches;
};

class listingObjWriter : public iObjWriter {
public:
   explicit listingObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual size_t reservePatch(const std::string& reason, size_t n);
   virtual void fillPatch(size_t k, const void *p);
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart();

private:
   void fmtData(std::ostream& o, const void *p, size_t n);

   std::unique_ptr<iObjWriterSink> m_pS;
   std::map<size_t,size_t> m_patchSizes;
};

class compositeObjWriter : public iObjWriter {
public:
   compositeObjWriter() : m_nextKey(0) {}
   ~compositeObjWriter();

   void sink(iObjWriter& o) { m_o.push_back(&o); }
   virtual size_t reservePatch(const std::string& reason, size_t n);
   virtual void fillPatch(size_t k, const void *p);
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart();

private:
   std::list<iObjWriter*> m_o;

   size_t m_nextKey;
   std::map<size_t,std::map<iObjWriter*,size_t> > m_patches;
};

class lineWriter {
public:
   explicit lineWriter(iObjWriter& o) : m_o(o), m_lineNumber(1) {}
   void setLineNumber(size_t l) { m_lineNumber = l; }
   void write(const std::string& reason, const void *p, size_t n)
   { m_o.write(m_lineNumber,reason,p,n); }
   iObjWriter& under() { return m_o; }

private:
   iObjWriter& m_o;
   size_t m_lineNumber;
};

} // namespace shtasm
