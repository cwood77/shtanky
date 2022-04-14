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
   shlinkTest(instrStream& s, const std::string& outFile) : m_stream(s), m_outFile(outFile) {}

   shlinkTest& withObject(const std::string& oFile);

   shlinkTest& test();

private:
   instrStream& m_stream;
   std::string m_outFile;
   std::list<std::string> m_objFiles;
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

// a test that's neither the top (araceli) nor bottom (shlink)
class intermediateTest {
public:
   virtual void andLink(shlinkTest& t) = 0;

protected:
   explicit intermediateTest(instrStream& s) : m_stream(s) {}

   void recordFileForNextStage(const std::string& f) { m_filesForNextStage.push_back(f); }
   const std::list<std::string>& getFilesForNextStage() const { return m_filesForNextStage; }

   instrStream& m_stream;

private:
   std::list<std::string> m_filesForNextStage;
};

class liamTest : public intermediateTest {
public:
   liamTest(instrStream& s, const std::string& file);

   virtual void andLink(shlinkTest& t);
};

class shtasmTest : public intermediateTest {
public:
   shtasmTest(instrStream& s, const std::string& file);

   virtual void andLink(shlinkTest& t);
};
