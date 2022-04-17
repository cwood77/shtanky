#pragma once
#include <stdio.h>
#include <string>

namespace cmn {

class iObjReader {
public:
   virtual ~iObjReader() {}

   virtual void read(void *p, size_t n) = 0;
   virtual std::string readString() = 0;
   virtual void readThumbprint(const char *pExpected) = 0;

   template<class T> void read(T& d) { read(&d,sizeof(T)); }
   template<class T> T read() { T d; read(d); return d; }
};

class binFileReader : public iObjReader {
public:
   explicit binFileReader(const std::string& filePath);
   ~binFileReader();
   virtual void read(void *p, size_t n);
   virtual std::string readString();
   virtual void readThumbprint(const char *pExpected);

private:
   FILE *m_pFile;
};

} // namespace cmn
