#pragma once

#include "ast.hpp"
#include <list>
#include <map>
#include <memory>
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

class intLiteralReader : public iLexorPhase {
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
      kIntLiteral,
      _kFirstDerivedToken
   };

   lexorBase(const lexemeInfo *pTable, const char *buffer);
   virtual ~lexorBase();

   void advance();
   size_t getToken() { return m_state.token; }
   std::string getTokenName(size_t t);
   std::string getTokenName() { return getTokenName(m_state.token); }
   std::string getLexeme() { return m_state.lexeme; }
   unsigned long getLineNumber() { return m_state.lineNumber; }

   void demand(size_t t);
   void demandOneOf(size_t n, ...);
   void demandAndEat(size_t t) { demand(t); advance(); }
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

class nodeFactory {
public:
   explicit nodeFactory(lexorBase& l) : m_l(l) {}

   void deferAttribute(const std::string& attr)
   {
      m_attrs.push_back(attr);
   }

   template<class T>
   T *create()
   {
      std::unique_ptr<T> pNode(new T());
      pNode->lineNumber = m_l.getLineNumber();
      for(auto it=m_attrs.begin();it!=m_attrs.end();++it)
         pNode->attributes.insert(*it);
      m_attrs.clear();
      return pNode.release();
   }

   template<class T>
   T& appendNewChild(node& parent)
   {
      T *pRval = create<T>();
      parent.appendChild(*pRval);
      return *pRval;
   }

private:
   lexorBase& m_l;
   std::list<std::string> m_attrs;
};

} // namespace cmn
