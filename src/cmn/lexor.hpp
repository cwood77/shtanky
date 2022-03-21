#pragma once

#include <list>
#include <map>
#include <string>

namespace cmn {

class lexemeInfo {
public:
   enum kind {
      kSymbolic,
      kAlphanumeric,
      kEndOfTable
   };

   kind k;
   size_t token;
   const char *lexeme;
   const char *name;

   lexemeInfo(kind k, size_t t, const char *l, const char *n)
   : k(k), token(t), lexeme(l), name(n) {}
};

class lexorState {
public:
   explicit lexorState(const char *p);

   const char *pThumb;
   size_t token;
   std::string lexeme;
   size_t lineNumber;
};

// generally, order matters with phases (e.g. put string literals before whitespace)
class iLexorPhase {
public:
   virtual ~iLexorPhase() {}
   virtual void collectTerminators(std::string& t) const = 0;
   virtual void advance(lexorState& s) const = 0;
};

class stringLiteralReader : public iLexorPhase {
public:
   virtual void collectTerminators(std::string& t) const;
   virtual void advance(lexorState& s) const;
};

class whitespaceEater : public iLexorPhase {
public:
   virtual void collectTerminators(std::string& t) const;
   virtual void advance(lexorState& s) const;
};

class lexorBase {
public:
   enum {
      kEOI,
      kUnknown,
      kName,
      kStringLiteral,
      _kFirstDerivedToken
   };

   lexorBase(const lexemeInfo *pTable, const char *buffer);
   virtual ~lexorBase();

   void advance();
   size_t getToken() { return m_state.token; }
   std::string getTokenName(size_t t);
   std::string getTokenName() { return getTokenName(m_state.token); }
   std::string getLexeme() { return m_state.lexeme; }

   void demandOneOf(size_t t);
   void demandOneOf(size_t n, ...);
   void demandAndEat(size_t t);
   void error(const std::string& msg);

protected:
   void addPhase(iLexorPhase& p);

private:
   void processTable(const lexemeInfo *pTable);
   void runPhasesUntilSteady();
   void matchSymbolic();
   void matchAlphanumeric();

   lexorState m_state;
   std::list<iLexorPhase*> m_phases;
   std::map<std::string,const lexemeInfo*> m_symbolics;
   std::map<std::string,const lexemeInfo*> m_alphas;
   std::map<size_t,const lexemeInfo*> m_lexemeDict;
   std::string m_terminators;
};

} // namespace cmn
