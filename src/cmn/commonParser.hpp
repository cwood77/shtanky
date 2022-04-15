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
//           | <const> <file>
//           | <func> <file>
//           | <ref> <strlit>
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
// <const> ::== <name> ':' <type> <field-init> ';'
// <func> ::== <attributes> 'func' <name> '(' <name> ':' <type> ')' ';'
//
// ------------------------- procedural
// <body> ::== '{' <statements> '}'
// <statements> ::== <statement>
//                 | <body>
//                 | e
// <statement> ::== 'var' <var> ';'
//                | <invoke> ';'
//                | <call> ';'
//                | <assignment> ';'
// <var>       ::== <name> ':' <type>
//                | <name> ':' <type> '=' <rvalue>
//                | <name> '=' <rvalue>
// <invoke>     ::== <lvalue> '->' <name> '(' <passedArgList> ')'
// <call>       ::== <lvalue> '(' <passedArgList> ')'
// <assignment> ::== <lvalue> '=' <rvalue>
// <passedArgList> ::== <rvalue> ',' <passedArgList>
//                    | <rvalue>
//                    | e
//
// ------------------------- rval/lval   <-- TODO left off here
// <lvalue> ::== <name>
//             | <call>
//             | <invoke>
//             | <faccess>
//             | <index-op>
//
// <rvalue> ::== <string-literal>
//             | <bool-literal>
//             | <lvalue>
//             | <bop>
//
// <faccess>  ::== <lvalue> '->' <name>
// <index-op> ::== <lvalue> '[' <lvalue> ']'
// <bop> ::== <rvalue> '+'{,etc.} <rvalue>
//
//  so 1 + 2 + 3
//  would be:
//     +
//  1    +
//     2  3
//  ... which is right associative (i.e. 1 + (2 + 3)
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


// missing:
//  index ops      **
//  arith ops      **
//  log ops        **
//  bitwise ops    **
//  ptr deref      ????
//  new/delete     ????
//  field access   ????
//  branch
//  loop
//  throw/catch
//  ctor/dtor
//  string interp
//  int literal formats
//  assignment
//  paren grouping in exprs
//  fancy call arg passing
//  templates
//  gemplates
//  bool type
//  char type
//  negative values
//
// long term DEFERRED:
//  floating point

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
   void parseGlobalConst(fileNode& f);
   void parseGlobalFunc(fileNode& f);
   void parseMethodOrGlobalFuncFromOpenParen(node& n);
   void parseMethodOrGlobalFuncFromAfterOpenParen(node& n);
   void parseDecledArgList(node& owner);

   void parseSequence(node& owner);
   void parseStatements(node& owner);
   bool tryParseStatement(node& owner);
   void parseVar(node& owner);
   void parseInvoke(std::unique_ptr<node>& inst, node& owner);
   void parseCall(std::unique_ptr<node>& inst, node& owner);
   void parseAssignment(std::unique_ptr<node>& inst, node& owner);
   void parsePassedArgList(node& owner);

   node& parseLValue();
   node& parseLValuePrime(node& n);
   void parseRValue(node& owner) { parseRValue(owner,&owner); }
   void parseRValue(node& owner, node *pExprRoot);
   void parseBop(node& owner, node *pExprRoot);

   void parseType(node& owner);

   void parseAttributes();

   commonLexor& m_l;
};

} // namespace cmn
