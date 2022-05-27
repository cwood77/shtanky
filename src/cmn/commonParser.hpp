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
// ------------------------- global funcs
// <const> ::== 'const' <name> ':' <type> '=' <rvalue> ';'
// <func> ::== 'func' <name> '(' <method/func>
//
// ------------------------- procedural
// <body> ::== '{' <statements> <body> '}'
//           | '{' <statements> '}'
// <statements> ::== <statement>
//                 | e
// <statement> ::== 'var' <var>
//                | <if>
//                | <loop>
//                | <lvalue> '=' <assignment> ';'
//                | <lvalue> <call-and-friends> ';'
// <statement-or-body> ::== <statement>
//                        | <body>
// <var>       ::== <name> ':' <type> ';'
//                | <name> ':' <type> '=' <rvalue> ';'
//                | <name> '=' <rvalue> ';'
// <assignment> ::== <rvalue> ';'
//
// <if> ::== 'if' '(' <rvalue> ')' <statement-or-body> <if'>
// <if'> ::== 'else' <if>
//          | 'else' <statement-or-body>
//          | e
//
// <loop> ::== 'for' <loop'>
// <loop'> ::== <loop-name> '(' <rvalue> ')' <statement-or-body>
// <loop-name> ::== '[' '+' <name> ']'
//                | '[' '-' <name> ']'
//                | e
//
// ------------------------- L-value / R-value
// ..... we want left associativity generally
//
// <lvalue> ::== <name> <lvalue'>
// <lvalue'> ::== ':' <name> <lvalue'>          [fieldaccess]
//              | '[' <rvalue> ']' <lvalue'>    [index]
//              | e
//
// <rvalue> ::== <literal>                      [* shenanigans for l-assoc]
//             | <lvalue> <rvalue'>
//
// <rvalue'> ::== <call-and-friends>
//              | <bop>
//              | <lvalue'>
//
// <call-and-friends> ::== '->' <invoke> <rvalue'>
//                       | '->(' <passed-arg-list> ')' <rvalue'>
//                       | '(' <call> <rvalue'>
// <invoke>     ::== <name> '(' <passed-arg-list> ')'
// <call>       ::== <passed-arg-list> ')'
// <passed-arg-list> ::== <rvalue> ',' <passed-arg-list>
//                      | <rvalue>
//                      | e
// <bop> ::== '+'{,etc.} <rvalue>               [* shenanigans for l-assoc]
//
// ------------------------- literals
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
//  ptr deref      ????
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
   // ------------------------- file
   void parseFile(fileNode& f);

   // ------------------------- class
   void parseClass(fileNode& f, const std::string& genericTypeExpr);
   void parseClassBases(classNode& c);
   void parseClassMembers(classNode& c);
   void parseMemberKeywords(size_t& flags);
   void parseField(fieldNode& n);
   void parseMethodOrGlobalFuncFromOpenParen(node& n);
   void parseMethodOrGlobalFuncFromAfterOpenParen(node& n);
   void parseDecledArgList(node& owner);

   // ------------------------- global funcs
   void parseGlobalConst(fileNode& f);
   void parseGlobalFunc(fileNode& f);

   // ------------------------- procedural
   void parseSequence(node& owner);
   void parseStatements(node& owner);
   bool tryParseStatement(node& owner);
   void parseStatementOrBody(node& owner);
   void parseVar(node& owner);
   void parseAssignment(std::unique_ptr<node>& inst, node& owner);
   void parseIf(node& owner);
   void parseIfPrime(node& owner);
   void parseLoop(node& owner);
   void parseLoopPrime(forLoopNode& l);
   void parseLoopName(forLoopNode& l);

   // ------------------------- L-value / R-value
   node& parseLValue();
   node& parseLValuePrime(node& n);
   void parseRValue(node& owner) { parseRValue(owner,&owner); }
   void parseRValue(node& owner, node *pExprRoot);
   bool parseCallAndFriends(std::unique_ptr<node>& inst, node& owner, bool require);
   void parseInvoke(std::unique_ptr<node>& inst, node& owner);
   void parseCall(std::unique_ptr<node>& inst, node& owner);
   void parsePassedArgList(node& owner);
   void parseBop(node& owner, node *pExprRoot);

   // ------------------------- literals
   node *parseLiteral(node& owner);
   void parseStructLiteralPart(node& owner);

   // ------------------------- type
   void parseType(node& owner);

   // ------------------------- attributes
   void parseAttributes();

   // ------------------------- generics
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
