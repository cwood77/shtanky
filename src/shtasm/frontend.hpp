#pragma once
#include "../cmn/binWriter.hpp"
#include "../cmn/lexor.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace cmn { namespace tgt { class asmArgInfo; } };

namespace shtasm {

class iTableWriter;

class lineLexor {
public:
   explicit lineLexor(const std::string& file)
   : m_fileName(file), m_file(file.c_str()), m_line(0) {}

   bool isDone() const { return !m_file.good(); }
   std::string getNextLine();
   size_t getLineNumber() const { return m_line; }
   const std::string& getFileName() const { return m_fileName; }

private:
   const std::string m_fileName;
   std::ifstream m_file;
   unsigned long m_line;
};

class lineParser {
public:
   explicit lineParser(lineLexor& l) : m_l(l) {}

   const lineLexor& getLexor() const { return m_l; }
   void parseLine(std::string& label, std::vector<std::string>& words, std::string& comment, std::string& rawLine);

private:
   void eatWhitespace(const char*& pThumb);
   std::string trimTrailingWhitespace(const std::string& w);
   bool shaveOffPart(const char*& pThumb, char delim, std::string& part);

   lineLexor& m_l;
};

class argLexor : public cmn::lexorBase {
public:
   enum {
      kLBracket = _kFirstDerivedToken,
      kRBracket,

      kAsterisk,
      kHyphen,
      kPlus,

      kRegRax,
      kRegRbx,
      kRegRcx,
      kRegRdx,
      kRegRbp,
      kRegRsp,
      kRegRsi,
      kRegRdi,
      kRegR8,
      kRegR9,
      kRegR10,
      kRegR11,
      kRegR12,
      kRegR13,
      kRegR14,
      kRegR15,

      kLabel,
   };

   enum {
      kClassReg8     = (1 << 1 | kNoClass),
      kClassReg16    = (1 << 2 | kNoClass),
      kClassReg32    = (1 << 3 | kNoClass),
      kClassReg64    = (1 << 4 | kNoClass),
      kClassRegAny   = (1 << 5 | kNoClass),
      kClassType     = (1 << 6 | kNoClass),
   };

   explicit argLexor(const char *buffer);
};

// <arg> ::== <reg>
//          | <number>
//          | <label>
//          | <memExpr>
// <memExpr> ::== <typeExpr> '[' <reg> <scale> ']'
// <typeExpr> ::== '(' <typeDesig> ')'
//               | e
// <scale> ::== '+' <number> '*' <reg> <disp>
//            | '+' <reg> <disp>
//            | e
// <disp> ::== '+' <number>
//           | e
//
// arguably this class belongs in cmn too, as the class it parses (asmArgInfo) is already
// there.  I'm leaving it here for now, as I don't expect anybody to need parsing ability
// except the assembler, and the reasons asmArgInfo is in cmn is already speculative
class argParser {
public:
   explicit argParser(argLexor& l) : m_l(l), m_pAi(NULL) {}

   void parseArg(cmn::tgt::asmArgInfo& i);

private:
   void parseReg();
   void parseMemExpr();
   void parseScale();

   argLexor& m_l;
   cmn::tgt::asmArgInfo *m_pAi;
};

class dataLexor : public cmn::lexorBase {
public:
   enum {
      kB = _kFirstDerivedToken,
      kW,
      kDW,
      kQW,

      kRep,
      kLParen,
      kRParen,
   };

   enum {
      kIntLitType = (1 << 1 | kNoClass),
   };

   explicit dataLexor(const char *buffer);
};

// for data generation, this writer implements the 'rep' keyword
class repObjWriter : public cmn::iObjWriter {
public:
   explicit repObjWriter(cmn::iObjWriter& inner) : m_inner(inner), m_rep(1) {}

   virtual void write(const std::string& reason, const void *p, size_t n);
   virtual void nextPart() { m_inner.nextPart(); }
   virtual unsigned long tell() { return m_inner.tell(); }

   void setRep(unsigned long v) { m_rep = v; }

private:
   cmn::iObjWriter& m_inner;
   unsigned long m_rep;
};

// <data> ::== 'rep' '(' <int> ')' <req-repable-data>
//           | <name> <data>
//           | <opt-repable-data>
//
// <req-repable-data> ::== <string> <data>
//                       | <type> <int> <data>
//
// <opt-repable-data> ::== <req-repable-data>
//                       | EOI
//
class dataParser {
public:
   dataParser(dataLexor& l, iTableWriter& tw) : m_l(l), m_tw(tw), m_rep(1) {}

   void parse(cmn::iObjWriter& w);

private:
   void parseData(cmn::iObjWriter& w);
   void parseRepableData(cmn::iObjWriter& w, bool req);

   template<class T>
   void writeInt(cmn::iObjWriter& w, __int64 value)
   {
      w.write<T>("_intdata",static_cast<T>(value));
   }

   dataLexor& m_l;
   iTableWriter& m_tw;
   __int64 m_rep;
};

} // namespace shtasm
