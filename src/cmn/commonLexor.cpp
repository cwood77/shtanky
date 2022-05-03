#include "commonLexor.hpp"

namespace cmn {

static const lexemeInfo scanTable[] = {
   { lexemeInfo::kSymbolic,     commonLexor::kLBracket,    "[",          "L bracket"       },
   { lexemeInfo::kSymbolic,     commonLexor::kRBracket,    "]",          "R bracket"       },
   { lexemeInfo::kSymbolic,     commonLexor::kLParen,      "(",          "L paren"         },
   { lexemeInfo::kSymbolic,     commonLexor::kRParen,      ")",          "R paren"         },
   { lexemeInfo::kSymbolic,     commonLexor::kLBrace,      "{",          "L brace"         },
   { lexemeInfo::kSymbolic,     commonLexor::kRBrace,      "}",          "R brace"         },

   { lexemeInfo::kSymbolic,     commonLexor::kColon,       ":",          "colon"           },
   { lexemeInfo::kSymbolic,     commonLexor::kSemiColon,   ";",          "semicolon"       },
   { lexemeInfo::kSymbolic,     commonLexor::kArrow,       "->",         "arrow"           },
   { lexemeInfo::kSymbolic,     commonLexor::kArrowParen,  "->(",        "arrow+paren"     },
   { lexemeInfo::kSymbolic,     commonLexor::kEquals,      "=",          "equals"          },
   { lexemeInfo::kSymbolic,     commonLexor::kComma,       ",",          "comma"           },

   { lexemeInfo::kAlphanumeric, commonLexor::kInterface,   "interface",  "interface"       },
   { lexemeInfo::kAlphanumeric, commonLexor::kOverride,    "override",   "override"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kAbstract,    "abstract",   "abstract"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kStatic,      "static",     "static"          },

   { lexemeInfo::kAlphanumeric, commonLexor::kRef,         "ref",        "ref"             },
   { lexemeInfo::kAlphanumeric, commonLexor::kConst,       "const",      "const"           },
   { lexemeInfo::kAlphanumeric, commonLexor::kFunc,        "func",       "func"            },

   { lexemeInfo::kAlphanumeric, commonLexor::kClass,       "class",      "class"           },
   { lexemeInfo::kAlphanumeric, commonLexor::kVar,         "var",        "var"             },

   { lexemeInfo::kAlphanumeric, commonLexor::kStr,         "str",        "str"             },
   { lexemeInfo::kAlphanumeric, commonLexor::kVoid,        "void",       "void"            },
   { lexemeInfo::kAlphanumeric, commonLexor::kPtr,         "ptr",        "ptr"             },

   { lexemeInfo::kAlphanumeric, commonLexor::kPublic,      "public",     "public"          },
   { lexemeInfo::kAlphanumeric, commonLexor::kProtected,   "protected",  "protected"       },
   { lexemeInfo::kAlphanumeric, commonLexor::kPrivate,     "private",    "private"         },

   { lexemeInfo::kAlphanumeric, commonLexor::kBoolLiteral, "true",       "boolean literal" },
   { lexemeInfo::kAlphanumeric, commonLexor::kBoolLiteral, "false",      "boolean literal" },

   { lexemeInfo::kSymbolic,     commonLexor::kPlus,        "+",          "plus"            },

   { lexemeInfo::kAlphanumeric, commonLexor::kGeneric,     "generic",    "generic"         },
   { lexemeInfo::kAlphanumeric, commonLexor::kInstantiate, "instantiate","instantiate"     },
   { lexemeInfo::kAlphanumeric, commonLexor::kType,        "type",       "type"            },

   { lexemeInfo::kEndOfTable,   0,                         NULL,         NULL              }
};

// N.B. these are treated as alphanumeric because I distinguish between templates and
//      operators using spaces
static const lexemeInfo genericOpsAlpha[] = {
   { lexemeInfo::kAlphanumeric, commonLexor::kLessThan,    "<",          "less than"       },
   { lexemeInfo::kAlphanumeric, commonLexor::kGreaterThan, ">",          "greater than"    },

   { lexemeInfo::kEndOfTable,   0,                         NULL,         NULL              }
};

static const lexemeInfo genericOpsSymbolic[] = {
   { lexemeInfo::kSymbolic,     commonLexor::kLessThan,    "<",          "less than"       },
   { lexemeInfo::kSymbolic,     commonLexor::kGreaterThan, ">",          "greater than"    },

   { lexemeInfo::kEndOfTable,   0,                         NULL,         NULL              }
};

static const lexemeClassInfo classTable[] = {

   { commonLexor::kClassBop, "binary operator",
      (size_t[]){
         commonLexor::kPlus,
         0,
      } },

   { commonLexor::kNoClass, NULL, NULL },
};

commonLexor::commonLexor(const char *buffer, const size_t *pUnsupported, bool useGenericExprPhase)
: lexorBase(buffer)
{
   addPhase(*new stringLiteralReader());
   addPhase(*new intLiteralReader());
   if(useGenericExprPhase)
      addPhase(*new genericTypeReader());
   addPhase(*new whitespaceEater());
   addTable(scanTable,pUnsupported);
   if(useGenericExprPhase)
      addTable(genericOpsAlpha,pUnsupported);
   else
      addTable(genericOpsSymbolic,pUnsupported);
   addClasses(classTable);
}

genericTypeExprLexor::genericTypeExprLexor(const char *buffer)
: commonLexor(buffer,NULL,/*useGenericExprPhase*/false)
{
   advance();
}

} // namespace cmn
