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

   auto listFile = cmn::pathUtil::addExtension(m_outFile,"o.list");
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

araceliTest& araceliTest::wholeApp()
{
   m_pLTest.reset(new shlinkTest(m_stream,m_folder + "\\.app"));
   return *this;
}

araceliTest& araceliTest::expectLiamOf(const std::string& path)
{
   auto header = cmn::pathUtil::addExtension(path,cmn::pathUtil::kExtLiamHeader);
   auto source = cmn::pathUtil::addExtension(path,cmn::pathUtil::kExtLiamSource);

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

   auto asmFile = cmn::pathUtil::addExtension(file,"asm");

   s.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(asmFile,"expected-"))
      .withVariable(asmFile)
      .because("generated assembly");

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

   auto oFile = cmn::pathUtil::addExtension(file,"o");
   auto mcListFile = cmn::pathUtil::addExtension(file,"o.mclist");
   auto oListFile = cmn::pathUtil::addExtension(file,"o.list");

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
