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
   { lexemeInfo::kAlphanumeric, commonLexor::kVirtual,     "virtual",    "virtual"         },
   { lexemeInfo::kAlphanumeric, commonLexor::kOverride,    "override",   "override"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kAbstract,    "abstract",   "abstract"        },
   { lexemeInfo::kAlphanumeric, commonLexor::kStatic,      "static",     "static"          },

   { lexemeInfo::kAlphanumeric, commonLexor::kRef,         "ref",        "ref"             },
   { lexemeInfo::kAlphanumeric, commonLexor::kConst,       "const",      "const"           },
   { lexemeInfo::kAlphanumeric, commonLexor::kFunc,        "func",       "func"            },

   { lexemeInfo::kAlphanumeric, commonLexor::kClass,       "class",      "class"           },
   { lexemeInfo::kAlphanumeric, commonLexor::kVar,         "var",        "var"             },

   { lexemeInfo::kAlphanumeric, commonLexor::kStr,         "str",        "str"             },
   { lexemeInfo::kAlphanumeric, commonLexor::kBool,        "bool",       "bool"            },
   { lexemeInfo::kAlphanumeric, commonLexor::kInt,         "int",        "int"             },
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

   { lexemeInfo::kAlphanumeric, commonLexor::kIf,          "if",         "if"              },
   { lexemeInfo::kAlphanumeric, commonLexor::kWhile,       "while",      "while"           },

   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "base",       "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "throw",      "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "catch",      "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "using",      "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "namespace",  "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "scpoe",      "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "pragma",     "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "new",        "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "delete",     "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "out",        "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "break",      "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "continue",   "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "for",        "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "do",         "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "internal",   "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "component",  "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "library",    "reserved"        },
   { lexemeInfo::kAlphanumeric, commonLexor::k_Reserved,   "package",    "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "~",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "`",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "!",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "@",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "#",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "$",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "%",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "^",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "&",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "*",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "-",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "'",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "\\",         "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "/",          "reserved"        },
   { lexemeInfo::kSymbolic,     commonLexor::k_Reserved,   "?",          "reserved"        },

   { lexemeInfo::kEndOfTable,   0,                         NULL,         NULL              }
};

// lexors can be "generic aware" or "generic unaware".  Aware lexors see into generic
// expressions, while unaware ones see only a opaque blob.  Unaware lexors require spacing
// around < and > to see them (i.e. they are alphanumeric tokens)
//
// for example
//      return type<foo>;
//
// unaware:
//      kReturn kGenericTypeExpr kSemiColon
// aware
//      kReturn kName kLessThan kName kGreaterThan kSemiColon
//
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

commonLexor::commonLexor(const char *buffer, const size_t *pUnsupported, bool genericAware)
: lexorBase(buffer)
{
   addPhase(*new stringLiteralReader());
   addPhase(*new intLiteralReader());
   if(!genericAware)
      addPhase(*new genericTypeReader());
   addPhase(*new whitespaceEater());
   addTable(scanTable,pUnsupported);
   if(!genericAware)
      addTable(genericOpsAlpha,pUnsupported);
   else
      addTable(genericOpsSymbolic,pUnsupported);
   addClasses(classTable);
}

genericTypeExprLexor::genericTypeExprLexor(const char *buffer)
: commonLexor(buffer,NULL,/*genericAware*/true)
{
   advance();
}

} // namespace cmn
