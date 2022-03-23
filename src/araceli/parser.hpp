#pragma once

#include "../cmn/ast.hpp"
#include <memory>

namespace cmn {

class lexorBase;

class parserBase {
protected:
   explicit parserBase(lexorBase& l);

   nodeFactory m_nFac;
};

} // namespace cmn

namespace araceli {

class lexor;

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
// <method> ::== <member-keywords> <name> '(' <name> ':' <type> ')' ':' <type> <body>
//             | e
// <field> ::== <member-keywords> <name> ':' <type> <field-init> ';'
// <field-init> ::== '=' <rvalue>
//                 | e
// <member-keywords> ::== 'override' <member-keywords>
//                      | 'private' <member-keywords>
//                      | e
//
// ------------------------- procedural
// <body> ::== '{' <statements> '}'
// <statements> ::== <statement>
//                 | e
// <statement> ::== <invoke> ';'
// <invoke> ::== <lvalue> '->' <name> '(' <rvalue> ')'
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
//           | 'bool'
//
// ------------------------- attributes
// <attributes> ::== '[' <name> ']' <attributes>
//                 |  e
//
class parser : public cmn::parserBase {
public:
   explicit parser(lexor& l);

   std::unique_ptr<cmn::fileNode> parseFile();

private:
   void parseFile(cmn::fileNode& f);
   void parseClass(cmn::fileNode& f);
   void parseClassBases(cmn::classNode& c);
   void parseClassMembers(cmn::classNode& c);
   void parseMemberKeywords(size_t& flags);
   void parseMethod(cmn::methodNode& n);
   void parseField(cmn::fieldNode& n);

   void parseSequence(cmn::node& owner);
   void parseStatements(cmn::node& owner);
   bool tryParseStatement(cmn::node& owner);
   void parseInvoke(std::unique_ptr<cmn::node>& inst, cmn::node& owner);
   void parseCall(std::unique_ptr<cmn::node>& inst, cmn::node& owner);
   void parsePassedArgList(cmn::node& owner);
   cmn::node& parseLValue();
   void parseRValue(cmn::node& owner);

   void parseType(cmn::node& owner);

   void parseAttributes();

   lexor& m_l;
};

} // namespace araceli
