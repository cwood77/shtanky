#include "../cmn/lexor.hpp"

namespace araceli {

class lexor : public cmn::lexorBase {
public:
   enum {
      kLBracket = _kFirstDerivedToken,
      kRBracket,
      kLParen,
      kRParen,
      kLBrace,
      kRBrace,
      kColon,
      kSemiColon,
      kArrow,
      kClass,
      kOverride,
      kAbstract,
      kStr,
      kVoid,
      kPublic,
      kProtected,
      kPrivate,
      kBoolLiteral,
      kEquals,
   };

   explicit lexor(const char *buffer);
};

} // namespace araceli
