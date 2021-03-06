#pragma once
#include "throw.hpp"
#include <cstddef>
#include <list>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

namespace cmn {

class iObjWriterSink {
public:
   virtual ~iObjWriterSink() {}
   virtual void write(const void *p, size_t n) = 0;
};

class binFileWriter : public iObjWriterSink {
public:
   explicit binFileWriter(const std::string& filePath);
   ~binFileWriter();
   virtual void write(const void *p, size_t n);

private:
   FILE *m_pFile;
};

class binMemoryWriter : public iObjWriterSink {
public:
   explicit binMemoryWriter(std::unique_ptr<unsigned char[]>& block);
   ~binMemoryWriter();
   virtual void write(const void *p, size_t n);

private:
   std::unique_ptr<unsigned char[]>& m_block;
   std::vector<unsigned char> m_alloc;
   size_t m_s;
};

// sole purpose is to _not_ delete it's inner, so inner can be
// re-used
class singleUseWriterSink : public iObjWriterSink {
public:
   explicit singleUseWriterSink(iObjWriterSink& inner) : m_inner(inner) {}

   virtual void write(const void *p, size_t n) { m_inner.write(p,n); }

private:
   iObjWriterSink& m_inner;
};

class iObjWriter {
public:
   virtual ~iObjWriter() {}

   virtual void write(const std::string& reason, const void *p, size_t n) = 0;
   void writeString(const std::string& reason, const std::string& v)
   { write<size_t>(reason+":len",v.length()); write(reason+":bytes",v.c_str(),v.length()); }
   template<class T> void write(const std::string& reason, const T& d)
   { write(reason,&d,sizeof(T)); }

   void writeCommentLine(const std::string& comment);
   void writeCommentLine(unsigned long line, const std::string& comment);

   virtual void nextPart() = 0;
   virtual unsigned long tell() = 0;
};

class retailObjWriter : public iObjWriter {
public:
   explicit retailObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual void write(const std::string& reason, const void *p, size_t n);
   virtual void nextPart() {}
   virtual unsigned long tell() { cdwTHROW("unimpled"); }

private:
   std::unique_ptr<iObjWriterSink> m_pS;
};

class listingObjWriter : public iObjWriter {
public:
   explicit listingObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual void write(const std::string& reason, const void *p, size_t n);
   virtual void nextPart();
   virtual unsigned long tell() { cdwTHROW("unimpled"); }

private:
   void fmtData(std::ostream& o, const void *p, size_t n);

   std::unique_ptr<iObjWriterSink> m_pS;
};

// designed for ODA (the online disassembler - onlinedisassembler.com)
class odaObjWriter : public iObjWriter {
public:
   explicit odaObjWriter(iObjWriterSink& s) : m_pS(&s) {}
   virtual void write(const std::string& reason, const void *p, size_t n);
   virtual void nextPart() {}
   virtual unsigned long tell() { cdwTHROW("unimpled"); }

private:
   void fmtData(std::ostream& o, const void *p, size_t n);

   std::unique_ptr<iObjWriterSink> m_pS;
};

class compositeObjWriter : public iObjWriter {
public:
   compositeObjWriter() : m_offset(0) {}
   ~compositeObjWriter();

   virtual void write(const std::string& reason, const void *p, size_t n);
   virtual void nextPart();
   virtual unsigned long tell() { return m_offset; }

   void sink(iObjWriter& o) { m_o.push_back(&o); }

   template<class T>
   void sinkNewFile(const std::string& path) { sink(*new T(*new binFileWriter(path))); }
   template<class T>
   void sinkExisting(iObjWriterSink& s) { sink(*new T(*new singleUseWriterSink(s))); }
   template<class T>
   void sinkMemory(std::unique_ptr<unsigned char[]>& block)
   { sink(*new T(*new binMemoryWriter(block))); }

   void sinkNewFileWithListing(const std::string& path);

private:
   std::list<iObjWriter*> m_o;
   unsigned long m_offset;
};

} // namespace cmn
