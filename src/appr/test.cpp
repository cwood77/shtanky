#include "../cmn/pathUtil.hpp"
#include "instr.hpp"
#include "test.hpp"

shlinkTest& shlinkTest::withObject(const std::string& oFile)
{
   m_objFiles.push_back(oFile);
   return *this;
}

shlinkTest& shlinkTest::test()
{
   m_stream.appendNew<doInstr>()
      .usingApp("bin\\out\\debug\\shlink.exe")
      .withArg(m_outFile)
      .withArgs(m_objFiles)
      .thenCheckReturnValue("link");

   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(m_outFile,"expected-"))
      .withVariable(m_outFile)
      .because("linked app");

   auto listFile = cmn::pathUtil::addExt(m_outFile,cmn::pathUtil::kExtList);
   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(listFile,"expected-"))
      .withVariable(listFile)
      .because("object listing");

   return *this;
}

araceliTest::araceliTest(instrStream& s, const std::string& folder)
: m_stream(s), m_folder(folder)
{
   m_stream.appendNew<doInstr>()
      .usingApp("bin\\out\\debug\\araceli.exe")
      .withArg(folder)
      .thenCheckReturnValue("araceli compile");

   m_stream.appendNew<compareInstr>()
      .withControl(folder + "\\expected-.build.bat")
      .withVariable(folder + "\\.build.bat")
      .because("generated batch file");
}

araceliTest::~araceliTest()
{
   if(m_pLTest)
      m_pLTest->test();
}

araceliTest& araceliTest::wholeApp()
{
   m_pLTest.reset(new shlinkTest(m_stream,m_folder + "\\.app"));

   shtasmTest(m_stream,".\\testdata\\sht\\oscall.asm").andLink(*m_pLTest.get());

   return *this;
}

araceliTest& araceliTest::skipLinkToWorkaroundHacks()
{
   m_pLTest.reset(NULL);
   return *this;
}

araceliTest& araceliTest::expectLiamOf(const std::string& path)
{
   auto header = cmn::pathUtil::addExt(path,cmn::pathUtil::kExtLiamHeader);
   auto source = cmn::pathUtil::addExt(path,cmn::pathUtil::kExtLiamSource);

   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(header,"expected-"))
      .withVariable(header)
      .because("generated liam header");

   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(source,"expected-"))
      .withVariable(source)
      .because("generated liam source");

   if(m_pLTest.get())
      liamTest(m_stream,source).andLink(*m_pLTest.get());

   return *this;
}

liamTest::liamTest(instrStream& s, const std::string& file)
: intermediateTest(s)
{
   s.appendNew<doInstr>()
      .usingApp("bin\\out\\debug\\liam.exe")
      .withArg(file)
      .thenCheckReturnValue("liam compile");

   auto asmFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtAsm);
   auto lirFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtLir);
   auto lirPostFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtLirPost);

   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(asmFile,"expected-"))
      .withVariable(asmFile)
      .because("generated assembly");
   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(lirFile,"expected-"))
      .withVariable(lirFile)
      .because("LIR early dump");
   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(lirPostFile,"expected-"))
      .withVariable(lirPostFile)
      .because("LIR late dump");

   recordFileForNextStage(asmFile);
}

void liamTest::andLink(shlinkTest& t)
{
   shtasmTest(m_stream,getFilesForNextStage().front()).andLink(t);
}

shtasmTest::shtasmTest(instrStream& s, const std::string& file)
: intermediateTest(s)
{
   s.appendNew<doInstr>()
      .usingApp("bin\\out\\debug\\shtasm.exe")
      .withArg(file)
      .thenCheckReturnValue("shtasm assemble");

   auto oFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtObj);
   auto mcListFile = cmn::pathUtil::addExt(oFile,cmn::pathUtil::kExtMcList);
   auto oListFile = cmn::pathUtil::addExt(oFile,cmn::pathUtil::kExtList);

   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(oFile,"expected-"))
      .withVariable(oFile)
      .because("object binary");

   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(mcListFile,"expected-"))
      .withVariable(mcListFile)
      .because("mc listing");

   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(oListFile,"expected-"))
      .withVariable(oListFile)
      .because("object listing");

   recordFileForNextStage(oFile);
}

void shtasmTest::andLink(shlinkTest& t)
{
   t.withObject(getFilesForNextStage().front());
}
