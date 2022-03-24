#include "lexor.hpp"

namespace cmn {

class commonLexor : public lexorBase {
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
      kOverride,
      kAbstract,
      kStatic,

      kClass,
      kVar,

      kStr,
      kVoid,

      kPublic,
      kProtected,
      kPrivate,

      kBoolLiteral,
   };

protected:
   commonLexor(const char *buffer, size_t *pUnsupported);
};

} // namespace cmn
