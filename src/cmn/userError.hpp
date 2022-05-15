#pragma once
#include "ast.hpp"
#include "global.hpp"
#include <list>
#include <sstream>
#include <string>

namespace cmn {

extern size_t kErrorInterfacesInherentlyPublicAbstract;
extern size_t kErrorInterfacesCannotHaveFields;
extern size_t kErrorMethodsStartingInheritanceMustBeAbstractOrVirtual;
extern size_t kErrorOverridesMustUseKeyword;
extern size_t kErrorNonVirtualMethodHidesInheritedMethod;
extern size_t kErrorFlagsIllegalOnNode;
extern size_t kErrorTooManyAccessSpecifiers;
extern size_t kErrorTooManyDynDispatchFlags;
extern size_t kErrorStaticCantBeUsedWithDynDispatch;
extern size_t kErrorOverrideCannotSpecifyAccess;
extern size_t kErrorFieldsBeginWithUnderscore;
extern size_t kErrorIllegalUseOfMember;

class userError {
public:
   void initialize(cmn::node& n, size_t _id) { pNode = &n; id = _id; }
   cmn::node *pNode;
   size_t id;
   std::stringstream stream;
};

class userErrors {
public:
   std::ostream& add(cmn::node& n, size_t id);

   void throwIfAny();

private:
   std::list<userError> m_errors;
};

extern timedGlobal<userErrors> gUserErrors;

} // namespace cmn
