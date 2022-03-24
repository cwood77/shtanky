#pragma once

#include "ast.hpp"
#include "lexor.hpp"
#include <memory>

namespace cmn {

class lexorBase;

class parserBase {
protected:
   explicit parserBase(lexorBase& l);

   nodeFactory m_nFac;
};

} // namespace cmn

namespace cmn {

class commonLexor;

// eBNF
//
// ------------------------- file
// <file> ::== <class> <file>
//           | <func> <file>
//           | e
//
// ------------------------- class
// <class> ::== <attributes> 'class' <name> <class-bases> '{' <class-members> '}'
// <class-bases> ::== ':' <name>
//                  | e
// <class-members> ::== <method> <class-members>
//                    | <field> <class-members>
//                    | e
// <member-keywords> ::== 'override' <member-keywords>
//                      | 'private' <member-keywords>
//                      | e
// <method> ::== <member-keywords> <name> '(' <name> ':' <type> ')' ':' <type> <body>
//             | e
// <field> ::== <member-keywords> <name> ':' <type> <field-init> ';'
// <field-init> ::== '=' <rvalue>
//                 | e
//
// ------------------------- global funcs (liam)
// <func> ::== <attributes> 'func' <name> '(' <name> ':' <type> ')' ';'
//
// ------------------------- procedural
// <body> ::== '{' <statements> '}'
// <statements> ::== <statement>
//                 | e
// <statement> ::== <invoke> ';'
//                | <call> ';'
// <invoke> ::== <lvalue> '->' <name> '(' <passedArgList> ')'
// <call> ::== <lvalue> '(' <passedArgList> ')'
// <passedArgList> ::== <rvalue> ',' <passedArgList>
//                    | <rvalue>
//                    | e
//
// ------------------------- rval/lval
// <lvalue> ::== <name>
// <rvalue> ::== <string-literal>
//             | <bool-literal>
//             | <lvalue>
//
// ------------------------- type
// <type> ::== 'str' '[' ']'
//           | 'void'
//           | <name> // user defined type
//
// ------------------------- attributes
// <attributes> ::== '[' <name> ']' <attributes>
//                 |  e
//
class commonParser : public parserBase {
public:
   explicit commonParser(commonLexor& l);

   std::unique_ptr<fileNode> parseFile();

private:
   void parseFile(fileNode& f);
   void parseClass(fileNode& f);
   void parseClassBases(classNode& c);
   void parseClassMembers(classNode& c);
   void parseMemberKeywords(size_t& flags);
   void parseField(fieldNode& n);
   void parseGlobalFunc(fileNode& f);
   void parseMethodOrGlobalFuncFromOpenParen(node& n);
   void parseDecledArgList(node& owner);

   void parseSequence(node& owner);
   void parseStatements(node& owner);
   bool tryParseStatement(node& owner);
   void parseInvoke(std::unique_ptr<node>& inst, node& owner);
   void parseCall(std::unique_ptr<node>& inst, node& owner);
   void parsePassedArgList(node& owner);

   node& parseLValue();
   void parseRValue(node& owner);

   void parseType(node& owner);

   void parseAttributes();

   commonLexor& m_l;
};

} // namespace cmn
