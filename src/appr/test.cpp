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

araceliTest::araceliTest(instrStream& s, const std::string& folder)
: m_stream(s), m_folder(folder)
{
   m_stream.appendNew<doInstr>()
      .usingApp("bin\\out\\debug\\araceli.exe")
      .withArg(folder);
}

araceliTest& araceliTest::wholeApp()
{
   m_pLTest.reset(new shlinkTest(m_stream,"folder-derived"));
   return *this;
}

araceliTest& araceliTest::expectLiamOf(const std::string& path)
{
   if(m_pLTest.get())
      liamTest(m_stream,path).andLink(*m_pLTest.get());

   return *this;
}
#endif
