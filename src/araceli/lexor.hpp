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
      kStr,
      kPublic,
      kProtected,
      kPrivate,
   };

   explicit lexor(const char *buffer);
};

} // namespace araceli
