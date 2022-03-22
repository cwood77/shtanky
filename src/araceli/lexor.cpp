#include "lexor.hpp"

namespace araceli {

static const cmn::lexemeInfo scanTable[] = {
   { cmn::lexemeInfo::kSymbolic,     lexor::kLBracket,    "[",        "L bracket"       },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRBracket,    "]",        "R bracket"       },
   { cmn::lexemeInfo::kSymbolic,     lexor::kLParen,      "(",        "L paren"         },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRParen,      ")",        "R paren"         },
   { cmn::lexemeInfo::kSymbolic,     lexor::kLBrace,      "{",        "L brace"         },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRBrace,      "}",        "R brace"         },

   { cmn::lexemeInfo::kSymbolic,     lexor::kColon,       ":",        "colon"           },
   { cmn::lexemeInfo::kSymbolic,     lexor::kSemiColon,   ";",        "semicolon"       },
   { cmn::lexemeInfo::kSymbolic,     lexor::kArrow,       "->",       "arrow"           },
   { cmn::lexemeInfo::kSymbolic,     lexor::kEquals,      "=",        "equals"          },
   { cmn::lexemeInfo::kSymbolic,     lexor::kComma,       ",",        "comma"           },

   { cmn::lexemeInfo::kAlphanumeric, lexor::kInterface,   "interface","interface"       },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kClass,       "class",    "class"           },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kOverride,    "override", "override"        },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kAbstract,    "abstract", "abstract"        },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kStatic,      "static",   "static"          },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kVar,         "var",      "var"             },

   { cmn::lexemeInfo::kAlphanumeric, lexor::kStr,         "str",      "str"             },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kVoid,        "void",     "void"            },

   { cmn::lexemeInfo::kAlphanumeric, lexor::kPublic,      "public",   "public"          },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kProtected,   "protected","protected"       },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kPrivate,     "private",  "private"         },

   { cmn::lexemeInfo::kAlphanumeric, lexor::kBoolLiteral, "true",     "boolean literal" },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kBoolLiteral, "false",    "boolean literal" },

   { cmn::lexemeInfo::kEndOfTable,   0,                   NULL,       NULL              }
};

lexor::lexor(const char *buffer)
: lexorBase(scanTable,buffer)
{
   addPhase(*new cmn::stringLiteralReader());
   addPhase(*new cmn::intLiteralReader());
   addPhase(*new cmn::whitespaceEater());
   advance();
}


} // namespace araceli
