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
      kVirtual,
      kOverride,
      kAbstract,
      kStatic,

      kRef,
      kConst,
      kFunc,

      kClass,
      kVar,

      kStr,
      kBool,
      kInt,
      kVoid,
      kPtr,

      kPublic,
      kProtected,
      kPrivate,

      kBoolLiteral,

      kPlus,

      kGeneric,
      kInstantiate,
      kType,

      kIf,
      kWhile,

      k_Reserved,

      kLessThan,
      kGreaterThan,

/* these are all modeled as reserved keywords for now
      // reserved - likely use later?
      kBase,
      kSelf, - allowed; araceli uses this to lower to liam
      kThrow,
      kCatch,
      kUsing,
      kNamespace,
      kScope,
      kPragma,
      kNew,
      kDelete,
      kOut,
      // reserved - maybe use later?
      kBreak,
      kContinue,
      kFor,
      kDo,
      kInternal,
      kComponent,
      kLibrary,
      kPackage,
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
      kSingleQuote,
      kBackslash,
      kForwardslash,
      kQuestion,
*/
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
