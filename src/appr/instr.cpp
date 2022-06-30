#include "../cmn/trace.hpp"
#include "instr.hpp"
#include "scriptWriter.hpp"
#include <fstream>

scriptState::scriptState()
: m_silenceExes(true), m_debug(true), m_progressCnt(0)
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

void scriptStream::silenceTestExeIf()
{
   if(m_pState->shouldSilenceExes())
      stream() << "-trace i >nul 2>&1" << std::endl;
   else
      stream() << std::endl;
}

void scriptStream::captureTestExeOutIf(const std::string& log)
{
   if(m_pState->shouldSilenceExes())
      stream() << " >" << log << " 2>&1" << std::endl;
   else
   {
      stream() << std::endl;

      // if we aren't capturing the log, then we don't want false positives later
      // should somebody diff the log and see it's unchanged.
      // so we'll change it just in case.
      std::ofstream l(log.c_str());
      ::srand(::time(NULL));
      l << "not captured " << ::rand() << std::endl;
   }
}

void scriptStream::updateProgress()
{
   if(!m_pState->shouldSilenceExes()) return;
   stream() << "echo | set /P=\".\"" << std::endl;
   m_pState->incrementTotalProgress();
}

void scriptStream::stopProgressDisplayForOutput()
{
   if(!m_pState->shouldSilenceExes()) return;
   stream() << "echo." << std::endl;
}

scriptStream& script::get(size_t i)
{
   auto& s = m_streams[i];
   s.configureIf(m_state);
   return s;
}

void script::skipTests(bool v)
{
   //get(kStreamClean).skip(v);
   get(kStreamCmd).skip(v);
   get(kStreamCheck).skip(v);
   get(kStreamBless).skip(v);
   get(kStreamUnbless).skip(v);
   m_hadSkips = true;
}

std::string script::buildAppPath(const std::string& exeName)
{
   std::string rval("bin\\out\\");
   if(m_state.isDebugMode())
      rval += "debug\\";
   else
      rval += "release\\";
   rval += exeName;
   return rval;
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

doInstr& doInstr::usingApp(const std::string& exeName)
{
   s().get(kStreamCmd).updateProgress();
   s().get(kStreamCmd).stream() << "\"" << s().buildAppPath(exeName) << "\" ";
   return *this;
}

doInstr& doInstr::withArg(const std::string& arg)
{
   s().get(kStreamCmd).stream() << "\"" << arg << "\" ";
   return *this;
}

doInstr& doInstr::withArgs(const std::list<std::string>& args)
{
   for(auto it=args.begin();it!=args.end();++it)
      this->withArg(*it);

   return *this;
}

doInstr& doInstr::thenCheckReturnValueAndCaptureOutput(const std::string& log, const std::string& errorHint)
{
   auto& cmds = s().get(kStreamCmd);
   auto& _s = cmds.stream();

   if(log.empty())
      cmds.silenceTestExeIf();
   else
      cmds.captureTestExeOutIf(log);

   auto passLbl = cmds.reserveLabel("pass");
   auto failLbl = cmds.reserveLabel("fail");
   _s << "if not %errorlevel% == 0 goto " << failLbl << std::endl;
   _s << "goto " << passLbl << std::endl;

   _s << ":" << failLbl << std::endl;
   cmds.stopProgressDisplayForOutput();
   _s << "echo FAIL: nonzero exit code doing " << errorHint << " [" << failLbl << "]" << std::endl;
   _s << "echo test: " << cmds.getFailHint() << std::endl;
   _s << "goto end" << std::endl;

   _s << ":" << passLbl << std::endl;

   markComplete();
   return *this;
}

compareInstr& compareInstr::withControl(const std::string& path)
{
   {
      auto& ss = s().get(kStreamCheck);
      ss.updateProgress();
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
      auto& ss = s().get(kStreamClean);
      ss.stream() << "del \"" << path << "\" >nul 2>&1" << std::endl;
   }

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
      ss.stopProgressDisplayForOutput();
      ss.stream() << "echo FAIL: files are different for " << reason << std::endl;
      ss.stream() << "echo       aborting run prematurely [" << failLbl << "]" << std::endl;
      ss.stream() << "echo test: " << ss.getFailHint() << std::endl;
      ss.stream() << "goto end" << std::endl;
      ss.stream() << ":" << passLbl << std::endl;
   }

   markComplete();
   return *this;
}
