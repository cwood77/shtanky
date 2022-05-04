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
      kArrowParen,
      kEquals,
      kComma,

      kInterface,
      kOverride,
      kAbstract,
      kStatic,

      kRef,
      kConst,
      kFunc,

      kClass,
      kVar,

      kStr,
      kVoid,
      kPtr,

      kPublic,
      kProtected,
      kPrivate,

      kBoolLiteral,

#if 0
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
#endif
      kPlus,
#if 0
      kSingleQuote,
      kBackslash,
      kForwardslash,
#endif
      kLessThan,
      kGreaterThan,
#if 0
      kQuestion,
#endif

      kGeneric,
      kInstantiate,
      kType,
   };

   enum {
      kClassBop = (1 << 0 | kNoClass)
   };

protected:
   commonLexor(const char *buffer,
      const size_t *pUnsupported, bool genericAware = false);
};

class genericTypeExprLexor : public commonLexor {
public:
   explicit genericTypeExprLexor(const char *buffer);
};

} // namespace cmn
