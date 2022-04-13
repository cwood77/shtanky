#pragma once
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>

// tests are like 'template' of instructions for certain operations (like araceli)
// tests do not emit (or complete emitting) instructions until asked

class instrStream;

class shlinkTest {
public:
   shlinkTest(instrStream& s, const std::string& outFile);

   shlinkTest& withObject(const std::string& oFile);

   shlinkTest& test();

private:
   doInstr& m_dI;
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

class liamTest {
public:
   liamTest(instrStream& s, const std::string& file);
};

class shtasmTest {
public:
   shtasmTest(instrStream& s, const std::string& file);
};

#if 0
class intermediateTest {
public:
   virtual void andLink(shlinkTest& s) = 0;

protected:
   explicit intermediateTest(instrStream& s);
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
