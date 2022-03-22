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
      kEquals,
      kComma,

      kInterface,
      kClass,
      kOverride,
      kAbstract,
      kStatic,
      kVar,

      kStr,
      kVoid,

      kPublic,
      kProtected,
      kPrivate,

      kBoolLiteral,
   };

   explicit lexor(const char *buffer);
};

} // namespace araceli
