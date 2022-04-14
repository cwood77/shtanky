#include "../cmn/trace.hpp"
#include "instr.hpp"
#include "scriptWriter.hpp"

scriptState::scriptState()
{
   scriptWriter::populateReservedLabels(*this);
}

std::string scriptState::reserveLabel(const std::string& hint)
{
   for(size_t i=0;;i++)
   {
      std::stringstream stream;
      stream << hint;
      if(i)
         stream << (i-1);
      std::string cpy = stream.str();

      if(m_labels.find(cpy) != m_labels.end())
         continue;

      m_labels.insert(cpy);
      return cpy;
   }
}

scriptStream& script::get(size_t i)
{
   auto& s = m_streams[i];
   s.configureIf(m_state);
   return s;
}

instr::~instr()
{
   if(!m_complete)
   {
      cdwINFO("user error: instruction was never completed\n");
      exit(-2);
   }
}

instrStream::~instrStream()
{
   for(auto it=m_instrs.begin();it!=m_instrs.end();++it)
      delete *it;
}

doInstr& doInstr::usingApp(const std::string& path)
{
   s().get(kStreamCmd).stream() << "\"" << path << "\" ";
   return *this;
}

doInstr& doInstr::withArg(const std::string& arg)
{
   s().get(kStreamCmd).stream() << "\"" << arg << "\" ";
   return *this;
}

doInstr& doInstr::thenCheckReturnValue(const std::string& errorHint)
{
   auto& cmds = s().get(kStreamCmd);
   auto& _s = cmds.stream();

   _s << " >nul 2>&1" << std::endl;

   auto passLbl = cmds.reserveLabel("pass");
   auto failLbl = cmds.reserveLabel("fail");
   _s << "if not %errorlevel% == 0 goto " << failLbl << std::endl;
   _s << "goto " << passLbl << std::endl;

   _s << ":" << failLbl << std::endl;
   _s << "echo FAIL: nonzero exit code doing " << errorHint << std::endl;
   _s << "goto end" << std::endl;

   _s << ":" << passLbl << std::endl;

   markComplete();
   return *this;
}

compareInstr& compareInstr::withControl(const std::string& path)
{
   {
      auto& ss = s().get(kStreamCheck);
      ss.stream() << "fc /b \"" << path << "\" ";
   }

   // args are backwards for bless stream, so just stash for now
   m_controlPath = path;

   {
      auto& ss = s().get(kStreamUnbless);
      ss.stream() << "git restore \"" << path << "\"" << std::endl;
   }

   return *this;
}

compareInstr& compareInstr::withVariable(const std::string& path)
{
   {
      auto& ss = s().get(kStreamCheck);
      ss.stream() << "\"" << path << "\" >nul 2>&1" << std::endl;
   }

   {
      auto& ss = s().get(kStreamBless);
      ss.stream()
         << "copy /Y \"" << path << "\" " << "\"" << m_controlPath << "\" >nul" << std::endl;
   }

   return *this;
}

compareInstr& compareInstr::because(const std::string& reason)
{
   {
      auto& ss = s().get(kStreamCheck);

      auto passLbl = ss.reserveLabel("pass");
      auto failLbl = ss.reserveLabel("fail");
      ss.stream() << "if not %errorlevel% == 0 goto " << failLbl << std::endl;
      ss.stream() << "goto " << passLbl << std::endl;

      ss.stream() << ":" << failLbl << std::endl;
      ss.stream() << "echo FAIL: files are different for " << reason << std::endl;
      ss.stream() << "echo       aborting run prematurely" << std::endl;
      ss.stream() << "goto end" << std::endl;
      ss.stream() << ":" << passLbl << std::endl;
   }

   markComplete();
   return *this;
}