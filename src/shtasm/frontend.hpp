#pragma once
#include "../cmn/lexor.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace cmn { namespace tgt { class asmArgInfo; } };

namespace shtasm {

class lexor {
public:
   explicit lexor(const std::string& file) : m_file(file.c_str()), m_line(0) {}

   bool isDone() const { return !m_file.good(); }
   std::string getNextLine();
   size_t getLineNumber() const { return m_line; }

private:
   std::ifstream m_file;
   unsigned long m_line;
};

class parser {
public:
   explicit parser(lexor& l) : m_l(l) {}

   const lexor& getLexor() const { return m_l; }
   void parseLine(std::string& label, std::vector<std::string>& words, std::string& comment, std::string& rawLine);

private:
   void eatWhitespace(const char*& pThumb);
   std::string trimTrailingWhitespace(const std::string& w);
   bool shaveOffPart(const char*& pThumb, char delim, std::string& part);

   lexor& m_l;
};

class fineLexor : public cmn::lexorBase {
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

   explicit fineLexor(const char *buffer);
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
class fineParser {
public:
   explicit fineParser(fineLexor& l) : m_l(l), m_pAi(NULL) {}

   void parseArg(cmn::tgt::asmArgInfo& i);

private:
   void parseReg();
   void parseMemExpr();
   void parseScale();

   fineLexor& m_l;
   cmn::tgt::asmArgInfo *m_pAi;
};

} // namespace shtasm
