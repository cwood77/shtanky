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

class lexemeClassInfo {
public:
   size_t Class;
   const char *name;

   size_t *pTokens;

   lexemeClassInfo(size_t Class, const char *name, size_t pTokens[])
   : Class(Class), name(name), pTokens(pTokens) {}
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
      _kFirstDerivedToken,
      kNoClass            = (size_t)1<<20, // also, first class
   };

   explicit lexorBase(const char *buffer);
   virtual ~lexorBase();

   void advance();
   size_t getToken() { return m_state.token; }
   size_t getTokenClass(size_t t);
   size_t getTokenClass() { return getTokenClass(getToken()); }
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
   void addTable(const lexemeInfo *pTable, const size_t *pUnsupported);
   void addClasses(const lexemeClassInfo *pClasses);

private:
   void runPhasesUntilSteady();
   void matchSymbolic();
   void matchAlphanumeric();

   lexorState m_state;
   std::list<iLexorPhase*> m_phases;
   std::map<std::string,const lexemeInfo*> m_symbolics;
   std::map<std::string,const lexemeInfo*> m_alphas;
   std::map<size_t,const lexemeInfo*> m_lexemeDict;
   std::set<size_t> m_unsupported;
   std::string m_terminators;
   std::map<size_t,size_t> m_tokenToClassMap;
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
