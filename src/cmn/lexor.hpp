#pragma once

// the base lexor allows several extension points
//  - you choose which phases you want to use (e.g. string literals)
//  - you plug in your own tokens and token classes
//  - you can mark 'unsupported' a known token
//    unsupported tokens are still 'known' by the lexor (e.g. getTokenName works), but
//    they will never be returned by an 'advance()', so a parser can still parse them
//    but know those parse routines are effectively deadcode.

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
};

class lexemeClassInfo {
public:
   size_t Class;
   const char *name;

   size_t *pTokens;
};

class lexorState {
public:
   explicit lexorState(const char *p);

   const char *pThumb;
   size_t token;
   std::string lexeme;
   size_t lineNumber;
   std::string fileName;
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
   virtual void collectTerminators(std::string& t) const {}
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

   // used in error reported; treated as optional
   void setFileName(const std::string& fileName) { m_state.fileName = fileName; }

   void advance();
   size_t getToken() { return m_state.token; }
   size_t getTokenClass(size_t t);
   size_t getTokenClass() { return getTokenClass(getToken()); }
   std::string getTokenName(size_t t); // this works for classes too
   std::string getTokenName() { return getTokenName(m_state.token); }
   std::string getLexeme() { return m_state.lexeme; }
   static __int64 getLexemeInt(const std::string& str);
   __int64 getLexemeInt() const { return getLexemeInt(m_state.lexeme); }
   static char getLexemeIntSize(const __int64& v);
   char getLexemeIntSize() const { return getLexemeIntSize(getLexemeInt()); }
   unsigned long getLineNumber() { return m_state.lineNumber; }

   void demand(const char *f, unsigned long l, size_t t);
   void demandOneOf(const char *f, unsigned long l, size_t n, ...);
   void demandAndEat(const char *f, unsigned long l, size_t t)
   { demand(f,l,t); advance(); }
   void error(const char *f, unsigned long l, const std::string& msg);

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
   std::map<size_t,const char*> m_lexemeDict;
   std::set<size_t> m_unsupported;
   std::string m_terminators;
   std::map<size_t,size_t> m_tokenToClassMap;
};

// besides convenience, this class handles attaching metadata to nodes
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
