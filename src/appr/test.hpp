#pragma once
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#if 0
class instrStream;

class shlinkTest {
public:
   shlinkTest(instrStream& s, const std::string& outFile);

   shlinkTest& withObject(const std::string& oFile);

private:
   doInstr& m_dI;
};

class intermediateTest {
public:
   virtual void andLink(shlinkTest& s) = 0;

protected:
   explicit intermediateTest(instrStream& s);
};

class araceliTest {
public:
   araceliTest(instrStream& s, const std::string& folder);
   araceliTest& wholeApp();
   araceliTest& expectLiamOf(const std::string& path);

private:
   instrStream& m_stream;
   const std::string m_folder;
   std::unique_ptr<shlinkTest> m_pLTest;
};

class liamTest : public intermediateTest {
public:
   liamTest(instrStream& s, const std::string& file);

   virtual void andLink(shlinkTest& s);
};

class shtasmTest : public intermediateTest {
public:
   shtasmTest(instrStream& s, const std::string& file);

   virtual void andLink(shlinkTest& s);
};
#endif
