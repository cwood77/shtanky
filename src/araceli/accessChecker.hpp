#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class classCatalog;

//    -- type to type relationships --
// public     everybody, anywhere; unbounded
// protected  only my derived classes
// private    only myself (or my clones)
//    -- file to file relationships --
// component  only my file
// module     only my directory (and subdirs)
// package    only my dirtree
//       -- build relationships --
// binary     not exposed after link
// export     exposted after link

// two keywords are already present; one build and either one type or one file
// types use file and build keywords
// members can use type, file, and build keywords
class defaultBuildKeywordProvider;

// verify that class members' access protection is honored
// members are only accessed via invoke. fieldAccessors, or calls
class memberAccessChecker : public cmn::hNodeVisitor {
public:
   explicit memberAccessChecker(classCatalog& cc) : m_cc(cc) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::fieldAccessNode& n);

   virtual void _implementLanguage() {} // all

private:
   void enforce(cmn::node& site,
      cmn::classNode& refee, cmn::classNode& refer, size_t kind);
   bool isAnAncestor(cmn::classNode& refee, cmn::classNode& refer);

   classCatalog& m_cc;
};

// verify that class type access protection is honored
// types are accessed in all access above, PLUS, userType and class inherit, locals,
// and varRefs
class typeAccessChecker;

} // namespace araceli
