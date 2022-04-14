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

private:
   std::set<std::string> m_labels;
};

class scriptStream {
public:
   scriptStream() : m_pState(NULL) {}

   void configureIf(scriptState& s) { m_pState = &s; }

   std::ostream& stream() { return m_stream; }
   std::string reserveLabel(const std::string& hint) { return m_pState->reserveLabel(hint); }

   void playback(std::ostream& s) { s << m_stream.str(); }

private:
   std::stringstream m_stream;
   scriptState *m_pState;
};

enum {
   kStreamCmd,
   kStreamCheck,
   kStreamBless,
   kStreamUnbless,
};

class script {
public:
   scriptStream& get(size_t i);

private:
   scriptState m_state;
   std::map<size_t,scriptStream> m_streams;
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

   doInstr& usingApp(const std::string& path);
   doInstr& withArg(const std::string& arg);
   doInstr& thenCheckReturnValue(const std::string& errorHint);
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