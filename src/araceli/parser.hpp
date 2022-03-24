#pragma once

#include "../cmn/ast.hpp" // TODO
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
class parser : public parserBase {
public:
   explicit parser(commonLexor& l);

   std::unique_ptr<fileNode> parseFile();

private:
   void parseFile(fileNode& f); // extend for liam
   void parseClass(fileNode& f); // TODO hard to share b/c of 'interface' 'abstract'
   void parseClassBases(classNode& c); // araceli only
   void parseClassMembers(classNode& c); // limit for liam
   void parseMemberKeywords(size_t& flags); // araceli only
   void parseMethod(methodNode& n); // araceli only
   void parseField(fieldNode& n);

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
