#pragma once

#include "ast.hpp"
#include "lexor.hpp"
#include <memory>

namespace cmn {

class commonLexor;
class genericTypeExprLexor;
class lexorBase;

// eBNF
//
// ------------------------- file
// <file> ::== <attributes> <class> <file>
//           | <attributes> <const> <file>
//           | <attributes> <func> <file>
//           | <attributes> <ref> <strlit>
//           | <attributes> 'instantiate' <text> ';'
//           | e
//
// ------------------------- class
// <class> ::== <generic> 'class'** <name> <class-bases> '{' <class-members> '}'
//   'class'** = 'class' | 'interface' | 'abstract'
// <class-bases> ::== ':' <name>
//                  | e
// <class-members> ::== <attributes> <member-keywords> <name> '(' <method> <class-members>
//                    | <attributes> <member-keywords> <name> ':' <field> <class-members>
//                    | e
// <member-keywords> ::== ['override'|'abstract'|'static'|'public/protected/private']
//                             <member-keywords>
//                      | e
// <field> ::== <type> '=' <rvalue> ';'
//            | <type> ';'
// <method/func> ::== <decl-arg-list> ')' ':' <type> ';'
//                  | <decl-arg-list> ')' ':' <type> <body>
// <decl-arg-list> ::== <name> ':' <type> ',' <decl-arg-list>
//                    | <name> ':' <type>
//                    | e
//
// ------------------------- global funcs (liam)
// <const> ::== 'const' <name> ':' <type> '=' <rvalue> ';'
// <func> ::== 'func' <name> '(' <method/func>
//
// ------------------------- procedural
// <body> ::== '{' <statements> <body> '}'
//           | '{' <statements> '}'
// <statements> ::== <statement>
//                 | e
// <statement> ::== 'var' <var>
//                | <lvalue> '=' <assignment> ';'
//                | <lvalue> <call-end-friends> ';'
// <var>       ::== <name> ':' <type> ';'
//                | <name> ':' <type> '=' <rvalue> ';'
//                | <name> '=' <rvalue> ';'
// <invoke>     ::== <name> '(' <passed-arg-list> ')' ';'
// <call>       ::== <passed-arg-list> ')' ';'
// <assignment> ::== <rvalue> ';'
// <passed-arg-list> ::== <rvalue> ',' <passed-arg-list>
//                      | <rvalue>
//                      | e
//
// <lvalue> ::== <name> <lvalue'>
// <lvalue'> ::== ':' <name> <lvalue'>         [fieldaccess]
//              | '[' <rvalue> ']' <lvaue'>    [index]
//              | e
//
// <rvalue> ::== <literal>
//             | <lvalue>
//             | <lvalue> <call-and-friends>
//             | <literal> <bop>
//             | <lvalue> <bop>
//
// <call-and-friends> ::== '->' <invoke>
//                       | '->(' <passed-arg-list> ')'
//                       | '(' <call>
//
// <literal> ::== <string-literal>
//              | <bool-literal>
//              | <int-literal>
//              | <struct-literal>
//
// <struct-literal> ::== '{' <struct-literal-part> '}'
// <struct-literal-part> :: == <literal> <struct-literal-part-prime>
//                           | <name> <struct-literal-part-prime> [e.g. func ptr for vtbl]
//                           | e
// <struct-literal-part-prime> ::== ',' <struct-literal-part>
//                                | e
//
                           // <faccess>  ::== <lvalue> '->' <name>
                           // <index-op> ::== <lvalue> '[' <lvalue> ']'
// <bop> ::== '+'{,etc.} <rvalue>
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
//           | 'ptr'
//           | 'bool'
//           | 'int'
//           | <name>
//           | <generic-opaque>
//
// ------------------------- attributes
// <attributes> ::== '[' <name> ']' <attributes>
//                 |  e
//
// ------------------------- generics
// <generic> ::== word<payload>
//                word is nonempty alphanumerics
//                payload empty/nonempty anything
//                payload must have comma, or cannot have spaces
//   --delegate to generic parser--

// missing:
//  index ops      **
//  arith ops      **
//  log ops        **
//  bitwise ops    **
//  ptr deref      ????
//  field access   ????
//  string interp
//  int literal formats
//  paren grouping in exprs
//  fancy call arg passing
//  negative values

class commonParser {
public:
   explicit commonParser(commonLexor& l);

   std::unique_ptr<fileNode> parseFile();

private:
   void parseFile(fileNode& f);
   void parseClass(fileNode& f, const std::string& genericTypeExpr);
   void parseClassBases(classNode& c);
   void parseClassMembers(classNode& c);
   void parseMemberKeywords(size_t& flags);
   void parseField(fieldNode& n);
   void parseMethodOrGlobalFuncFromOpenParen(node& n);
   void parseMethodOrGlobalFuncFromAfterOpenParen(node& n);
   void parseDecledArgList(node& owner);

   void parseGlobalConst(fileNode& f);
   void parseGlobalFunc(fileNode& f);

   void parseSequence(node& owner);
   void parseStatements(node& owner);
   bool tryParseStatement(node& owner);
   void parseVar(node& owner);
   bool parseCallAndFriends(std::unique_ptr<node>& inst, node& owner, bool require);
   void parseInvoke(std::unique_ptr<node>& inst, node& owner);
   void parseCall(std::unique_ptr<node>& inst, node& owner);
   void parseAssignment(std::unique_ptr<node>& inst, node& owner);
   void parsePassedArgList(node& owner);

   node& parseLValue();
   node& parseLValuePrime(node& n);
   void parseRValue(node& owner) { parseRValue(owner,&owner); }
   void parseRValue(node& owner, node *pExprRoot);
   bool parseLiteral(node& owner);
   void parseStructLiteralPart(node& owner);
   void parseBop(node& owner, node *pExprRoot);

   void parseType(node& owner);

   void parseAttributes();
   void parseGeneric(node& n, const std::string& genericTypeExpr);

   nodeFactory m_nFac;
   commonLexor& m_l;
};

// <generic> ::== 'generic' '<' <constraints> '>'
//              | e
// <constraints> ::== <constraint> <constraints'>
// <constraints'> ::== ',' <constraints>
//                   | e
// <constraint> ::== <name> ':' <name>
//                 | <name>
//
//               --------
//
// <type-paramed> ::== <name> '<' <arg> '>'
// <arg> ::== <type> <args'>
//          | <type-paramed> <args'>
// <args'> :== ',' <arg>
//           | e
//
class genericTypeExprParser {
public:
   explicit genericTypeExprParser(genericTypeExprLexor& l) : m_l(l) {}

   void parseGeneric(genericNode& n);
   void parseConstraints(genericNode& n);
   void parseConstraint(genericNode& n);

   void parseTypeParamed(std::string& base, std::list<std::string>& args);

private:
   void parseTypeParamArgList(std::list<std::string>& args);
   void parseTypeParamArg(std::list<std::string>& args);

   genericTypeExprLexor& m_l;
};

} // namespace cmn
