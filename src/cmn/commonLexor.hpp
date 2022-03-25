#pragma once
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

      kRef,
      kFunc,

      kClass,
      kVar,

      kStr,
      kVoid,

      kPublic,
      kProtected,
      kPrivate,

      kBoolLiteral,

      // missing
      kBase,
      kSelf,
      kThrow,
      kCatch,
      kIf,
      kFor,
      kDo,
      kWhile,
      kBreak,
      kContinue,
      kUsing,
      kNamespace,
      kScope,
      kPragma,
      kNew,
      kDelete,
      kVirtual,

      kOut,
      //kRef,

      // reserved
      kTilde,
      kBacktick,
      kBang,
      kAt,
      kHash,
      kDollar,
      kPercent,
      kCarrot,
      kAmpersand,
      kAsterisk,
      kHyphen,
      kPlus,
      kSingleQuote,
      kBackslash,
      kForwardslash,
      kLessThan,
      kGreaterThan,
      kQuestion,
   };

   enum {
      kClassBop = (1 << 0 | kNoClass)
   };

protected:
   commonLexor(const char *buffer, const size_t *pUnsupported);
};

} // namespace cmn
