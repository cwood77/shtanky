#pragma once
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>

namespace shtasm {

class iObjWriterSink {
public:
   virtual ~iObjWriterSink() {}
   virtual void write(const void *p, size_t n) = 0;
   virtual int tell() = 0;
};

class binFileWriter : public iObjWriterSink {
public:
   explicit binFileWriter(const std::string& filePath);
   ~binFileWriter();
   virtual void write(const void *p, size_t n);
   virtual int tell();

private:
   FILE *m_pFile;
};

class iObjWriter {
public:
   virtual ~iObjWriter() {}

   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n) = 0;

   virtual void nextPart() = 0;
};

class retailObjWriter : public iObjWriter {
public:
   explicit retailObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart() {}

private:
   std::unique_ptr<iObjWriterSink> m_pS;
};

class listingObjWriter : public iObjWriter {
public:
   explicit listingObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart();

private:
   void fmtData(std::ostream& o, const void *p, size_t n);

   std::unique_ptr<iObjWriterSink> m_pS;
   std::map<size_t,size_t> m_patchSizes;
};

class compositeObjWriter : public iObjWriter {
public:
   ~compositeObjWriter();

   void sink(iObjWriter& o) { m_o.push_back(&o); }
   virtual void write(size_t lineNum, const std::string& reason, const void *p, size_t n);
   virtual void nextPart();

private:
   std::list<iObjWriter*> m_o;
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
