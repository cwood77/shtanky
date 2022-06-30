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
      .usingApp("shlink.exe")
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

testBase::~testBase()
{
   if(m_pLTest)
      m_pLTest->test();
}

void testBase::verifyAdditionalArtifact(const std::string& path, const std::string& cuz)
{
   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(path,"expected-"))
      .withVariable(path)
      .because(cuz);
}

void testBase::setupAutoShlink(const std::string& appPath)
{
   m_appPath = appPath;
   m_pLTest.reset(new shlinkTest(m_stream,appPath));

   // built-in asm files
   shtasmTest(m_stream,".\\testdata\\sht\\array.asm").andLink(*m_pLTest.get());
   shtasmTest(m_stream,".\\testdata\\sht\\flags.asm").andLink(*m_pLTest.get());
   shtasmTest(m_stream,".\\testdata\\sht\\oscall.asm").andLink(*m_pLTest.get());
   shtasmTest(m_stream,".\\testdata\\sht\\string.asm").andLink(*m_pLTest.get());
}

void testBase::emulateAndCheckOutput()
{
   if(m_pLTest)
   {
      m_pLTest->test();
      m_pLTest.reset(NULL);
   }

   auto log = cmn::pathUtil::addExt(m_appPath,"log");

   m_stream.appendNew<doInstr>()
      .usingApp("shtemu.exe")
      .withArg(m_appPath)
      .thenCheckReturnValueAndCaptureOutput(log,"Win64 emulation");

   m_stream.appendNew<compareInstr>()
      .withControl(cmn::pathUtil::addPrefixToFilePart(log,"expected-"))
      .withVariable(log)
      .because("execution output");
}

araceliTest::araceliTest(instrStream& s, const std::string& folder)
: testBase(s), m_folder(folder)
{
   m_stream.appendNew<doInstr>()
      .usingApp("araceli.exe")
      .withArg(folder)
      .thenCheckReturnValue("araceli compile");

   m_stream.appendNew<compareInstr>()
      .withControl(folder + "\\expected-.build.bat")
      .withVariable(folder + "\\.build.bat")
      .because("generated batch file");
}

araceliTest& araceliTest::useFullBuildStack()
{
   setupAutoShlink(m_folder + "\\.app");

   // include the standard library
   expectLiamOf(".\\testdata\\sht\\cons\\program.ara");
   expectLiamOf(".\\testdata\\sht\\core\\array.ara");
   expectLiamOf(".\\testdata\\sht\\core\\loopInst.ara");
   expectLiamOf(".\\testdata\\sht\\core\\object.ara");
   expectLiamOf(".\\testdata\\sht\\core\\string.ara");
   // unreferenced
   //expectLiamOf(".\\testdata\\sht\\mem\\map\\prims.ara");

   // include the generated target file
   expectLiamOf(m_folder + "\\.target.ara", /* hasPhilemon */ false);

   return *this;
}

araceliTest& araceliTest::expectLiamOf(const std::string& path, bool hasPhilemon)
{
   auto salome = cmn::pathUtil::addExt(path,"sa");
   auto philemon = cmn::pathUtil::addExt(path,"ph");
   auto header = cmn::pathUtil::addExt(path,cmn::pathUtil::kExtLiamHeader);
   auto source = cmn::pathUtil::addExt(path,cmn::pathUtil::kExtLiamSource);

   if(hasPhilemon)
   {
      m_stream.appendNew<compareInstr>()
         // salome files will get sucked in by philemon, so make sure these
         // aren't salome files
         .withControl(cmn::pathUtil::addExt(
            cmn::pathUtil::addPrefixToFilePart(salome,"expected-"),"txt"))
         .withVariable(salome)
         .because("generated salome");

      m_stream.appendNew<compareInstr>()
         // philemon files will get sucked in by araceli, so make sure these
         // aren't philemon files
         .withControl(cmn::pathUtil::addExt(
            cmn::pathUtil::addPrefixToFilePart(philemon,"expected-"),"txt"))
         .withVariable(philemon)
         .because("generated philemon");
   }

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
      .usingApp("liam.exe")
      .withArg(file)
      .thenCheckReturnValue("liam compile");

   auto asmFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtAsm);
   auto lirFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtLir);
   auto lirPostFile = cmn::pathUtil::addExt(file,cmn::pathUtil::kExtLirPost);

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
      .usingApp("shtasm.exe")
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

void testWriter::add(const std::string& name, std::function<void (instrStream&)> f)
{
   if(m_subset && !m_cl.getSwitch(name,"",false))
      return;

   m_s.noteFailHint(name);
   instrStream is(m_s);
   f(is);
}

void testWriter::skipByDefault(const std::string& name, std::function<void (instrStream&)> f)
{
   if(!m_subset || !m_cl.getSwitch(name,"",false))
      return;

   m_s.noteFailHint(name);
   instrStream is(m_s);
   f(is);
}
