#pragma once

#include <memory>
#include "ast.hpp"

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

   std::unique_ptr<fileNode> parseFile();

private:
   void parseFile(fileNode& f);
   void parseClass(fileNode& f);
   void parseClassBases(classNode& c);
   void parseClassMembers(classNode& c);

   void parseAttributes();

   lexor& m_l;
};

// compile process
// - inputs:
//   dir folders
//   src folder
//   ns to build
//   target type (e.g console EXE)
//
// -steps
//   list root nss
//   parse src
//   check unresolved refs
//   repeat
//   gen metadata
//   run target type codegen
//   lower
//   done

} // namespace araceli
