#include "../cmn/pathUtil.hpp"
#include "instr.hpp"
#include "test.hpp"

#if 0
shlinkTest::shlinkTest(instrStream& s, const std::string& outFile)
: m_dI(s.appendNew<doInstr>())
{
   m_dI.usingApp("bin\\out\\debug\\shlink.exe")
      .withArg(outFile);
}

shlinkTest& shlinkTest::withObject(const std::string& oFile)
{
   m_dI.withArg(oFile);
   return *this;
}
#endif

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

/*
araceliTest& araceliTest::wholeApp()
{
   m_pLTest.reset(new shlinkTest(m_stream,"folder-derived"));
   return *this;
}
*/

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

   /*
   if(m_pLTest.get())
      liamTest(m_stream,path).andLink(*m_pLTest.get());
      */

   return *this;
}

liamTest::liamTest(instrStream& s, const std::string& file)
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
}

shtasmTest::shtasmTest(instrStream& s, const std::string& file)
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
}
