#include "commonLexor.hpp"

namespace cmn {

static const lexemeInfo scanTable[] = {
   { lexemeInfo::kSymbolic,     commonLexor::kLBracket,    "[",        "L bracket"       },
   { lexemeInfo::kSymbolic,     commonLexor::kRBracket,    "]",        "R bracket"       },
   { lexemeInfo::kSymbolic,     commonLexor::kLParen,      "(",        "L paren"         },
   { lexemeInfo::kSymbolic,     commonLexor::kRParen,      ")",        "R paren"         },
   { lexemeInfo::kSymbolic,     commonLexor::kLBrace,      "{",        "L brace"         },
   { lexemeInfo::kSymbolic,     commonLexor::kRBrace,      "}",        "R brace"         },

   { lexemeInfo::kSymbolic,     commonLexor::kColon,       ":",        "colon"           },
   { lexemeInfo::kSymbolic,     commonLexor::kSemiColon,   ";",        "semicolon"       },
   { lexemeInfo::kSymbolic,     commonLexor::kArrow,       "->",       "arrow"           },
   { lexemeInfo::kSymbolic,     commonLexor::kEquals,      "=",        "equals"          },
   { lexemeInfo::kSymbolic,     commonLexor::kComma,       ",",        "comma"           },

   { lexemeInfo::kAlphanumeric, commonLexor::kInterface,   "interface","interface"       },
   { lexemeInfo::kAlphanumeric, commonLexor::kOverride,    "override", "override"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kAbstract,    "abstract", "abstract"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kStatic,      "static",   "static"          },

   { lexemeInfo::kAlphanumeric, commonLexor::kClass,       "class",    "class"           },
   { lexemeInfo::kAlphanumeric, commonLexor::kVar,         "var",      "var"             },

   { lexemeInfo::kAlphanumeric, commonLexor::kStr,         "str",      "str"             },
   { lexemeInfo::kAlphanumeric, commonLexor::kVoid,        "void",     "void"            },

   { lexemeInfo::kAlphanumeric, commonLexor::kPublic,      "public",   "public"          },
   { lexemeInfo::kAlphanumeric, commonLexor::kProtected,   "protected","protected"       },
   { lexemeInfo::kAlphanumeric, commonLexor::kPrivate,     "private",  "private"         },

   { lexemeInfo::kAlphanumeric, commonLexor::kBoolLiteral, "true",     "boolean literal" },
   { lexemeInfo::kAlphanumeric, commonLexor::kBoolLiteral, "false",    "boolean literal" },

   { lexemeInfo::kEndOfTable,   0,                   NULL,       NULL              }

};

commonLexor::commonLexor(const char *buffer, size_t *pUnsupported)
: lexorBase(buffer)
{
   addPhase(*new stringLiteralReader());
   addPhase(*new intLiteralReader());
   addPhase(*new whitespaceEater());
   addTable(scanTable,pUnsupported);
}

} // namespace cmn
