#pragma once
#include "../cmn/cmdline.hpp"
#include <functional>
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
   shlinkTest(instrStream& s, const std::string& outFile)
   : m_stream(s), m_outFile(outFile) {}

   shlinkTest& withObject(const std::string& oFile);

   shlinkTest& test();

private:
   instrStream& m_stream;
   std::string m_outFile;
   std::list<std::string> m_objFiles;
};

class testBase {
public:
   virtual ~testBase();

protected:
   explicit testBase(instrStream& s) : m_stream(s) {}

   virtual void setupAutoShlink(const std::string& appPath);
   void emulateAndCheckOutput();

   instrStream& m_stream;
   std::unique_ptr<shlinkTest> m_pLTest;

private:
   std::string m_appPath;
};

class araceliTest : public testBase {
public:
   araceliTest(instrStream& s, const std::string& folder);

   araceliTest& wholeApp() { setupAutoShlink(m_folder + "\\.app"); return *this; }
   araceliTest& expectLiamOf(const std::string& path, bool hasPhilemon = true);

private:
   const std::string m_folder;
};

// a test that's neither the top (araceli) nor bottom (shlink)
class intermediateTest : public testBase {
public:
   virtual void andLink(shlinkTest& t) = 0;

protected:
   explicit intermediateTest(instrStream& s) : testBase(s) {}

   virtual void setupAutoShlink(const std::string& appPath)
   { testBase::setupAutoShlink(appPath); andLink(*m_pLTest.get()); }

   void recordFileForNextStage(const std::string& f) { m_filesForNextStage.push_back(f); }
   const std::list<std::string>& getFilesForNextStage() const { return m_filesForNextStage; }

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

   shtasmTest& wholeApp(const std::string& appPath)
   { setupAutoShlink(appPath); return *this; }

   shtasmTest& emulateAndCheckOutput()
   { testBase::emulateAndCheckOutput(); return *this; }

   virtual void andLink(shlinkTest& t);
};

class testWriter {
public:
   testWriter(script& s, bool subset, cmn::cmdLine& cl)
   : m_s(s), m_subset(subset), m_cl(cl) {}
   void add(const std::string& name, std::function<void (instrStream&)> f);

private:
   script& m_s;
   bool m_subset;
   cmn::cmdLine& m_cl;
};
