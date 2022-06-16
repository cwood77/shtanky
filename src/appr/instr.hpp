#pragma once
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>

// instructions in appriaser are how the system talks to the script.
// instructions serialize into the stream immediately.

class instrStream;

class scriptState {
public:
   scriptState();

   std::string reserveLabel(const std::string& hint);

   void setSilentExes(bool v) { m_silenceExes = v; }
   bool shouldSilenceExes() const { return m_silenceExes; }
   void setDebugMode(bool v) { m_debug = v; }
   bool isDebugMode() const { return m_debug; }

   void incrementTotalProgress() { m_progressCnt++; }
   size_t getTotalProgressSize() const { return m_progressCnt; }

   std::string failHint;

private:
   std::set<std::string> m_labels;
   bool m_silenceExes;
   bool m_debug;
   size_t m_progressCnt;
};

class scriptStream {
public:
   scriptStream() : m_pState(NULL), m_skipped(false) {}

   void configureIf(scriptState& s) { m_pState = &s; }
   void skip(bool v = true) { m_skipped = v; }

   std::ostream& stream() { return m_skipped ? m_fakeStream : m_stream; }
   std::string reserveLabel(const std::string& hint) { return m_pState->reserveLabel(hint); }
   void silenceTestExeIf();
   void captureTestExeOutIf(const std::string& log);

   void updateProgress();
   void stopProgressDisplayForOutput();

   void playback(std::ostream& s) { s << m_stream.str(); }

   const std::string& getFailHint() const { return m_pState->failHint; }

private:
   std::stringstream m_stream;
   std::stringstream m_fakeStream;
   scriptState *m_pState;
   bool m_skipped;
};

enum {
   kStreamClean,
   kStreamCmd,
   kStreamCheck,
   kStreamBless,
   kStreamUnbless,
};

class script {
public:
   explicit script(bool silentExes, bool debug = true) : m_hadSkips(false)
   { m_state.setSilentExes(silentExes);
     m_state.setDebugMode(debug); }

   size_t getTotalProgressSize() const { return m_state.getTotalProgressSize(); }

   scriptStream& get(size_t i);

   void skipTests(bool v = true);
   bool hadSkips() const { return m_hadSkips; }

   void noteFailHint(const std::string& hint) { m_state.failHint = hint; }

   std::string buildAppPath(const std::string& exeName);

private:
   scriptState m_state;
   std::map<size_t,scriptStream> m_streams;
   bool m_hadSkips;
};

class instr {
public:
   virtual ~instr();

protected:
   explicit instr(script& s) : m_script(s), m_complete(false) {}

   script& s() { return m_script; }

   void markComplete() { m_complete = true; }

private:
   script& m_script;
   bool m_complete;
};

class instrStream {
public:
   explicit instrStream(script& s) : m_s(s) {}
   ~instrStream();

   template<class T>
   T& appendNew()
   {
      m_instrs.push_back(new T(m_s));
      return dynamic_cast<T&>(*m_instrs.back());
   }

private:
   script& m_s;
   std::list<instr*> m_instrs;
};

class doInstr : public instr {
public:
   explicit doInstr(class script& s) : instr(s) {}

   doInstr& usingApp(const std::string& exeName);
   doInstr& withArg(const std::string& arg);
   doInstr& withArgs(const std::list<std::string>& args);
   doInstr& thenCheckReturnValue(const std::string& errorHint)
   { return thenCheckReturnValueAndCaptureOutput("",errorHint); }
   doInstr& thenCheckReturnValueAndCaptureOutput(
      const std::string& log, const std::string& errorHint);
};

class compareInstr : public instr {
public:
   explicit compareInstr(class script& s) : instr(s) {}

   compareInstr& withControl(const std::string& path);
   compareInstr& withVariable(const std::string& path);

   compareInstr& because(const std::string& reason);

private:
   std::string m_controlPath;
};
