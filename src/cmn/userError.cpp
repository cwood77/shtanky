#include "throw.hpp"
#include "userError.hpp"

namespace cmn {
namespace {

class errorSorter {
public:
   bool operator()(const userError *lhs, const userError *rhs) const
   {
      auto& lhsF = lhs->pNode->getAncestor<cmn::fileNode>();
      auto& rhsF = rhs->pNode->getAncestor<cmn::fileNode>();

      if(lhsF.fullPath != rhsF.fullPath)
         return lhsF.fullPath < rhsF.fullPath;

      if(lhs->pNode->lineNumber != rhs->pNode->lineNumber)
         return lhs->pNode->lineNumber < rhs->pNode->lineNumber;

      if((const char*)lhs->id != (const char*)rhs->id)
         return lhs->id < rhs->id;

      return lhs < rhs;
   }
};

} // anonymous namespace

size_t kErrorInterfacesInherentlyPublicAbstract = (size_t)"interface members are inherently public abstract and should not be marked";
size_t kErrorInterfacesCannotHaveFields = (size_t)"interfaces cannot have fields";
size_t kErrorMethodsStartingInheritanceMustBeAbstractOrVirtual = (size_t)"methods starting inheritance must be abstract or virtual";
size_t kErrorOverridesMustUseKeyword = (size_t)"overrides must use override keyword";
size_t kErrorNonVirtualMethodHidesInheritedMethod = (size_t)"non-virtual method hides an inherited method";
size_t kErrorFlagsIllegalOnNode = (size_t)"flags are illegal on node";
size_t kErrorTooManyAccessSpecifiers = (size_t)"too many access specifiers given";
size_t kErrorTooManyDynDispatchFlags = (size_t)"too many dynamic binding keywords given";
size_t kErrorStaticCantBeUsedWithDynDispatch = (size_t)"static can't be used with dynamic binding";
size_t kErrorOverrideCannotSpecifyAccess = (size_t)"overrides cannot have access specifiers";
size_t kErrorFieldsBeginWithUnderscore = (size_t)"field names should begin with an underscore";
size_t kErrorIllegalUseOfMember = (size_t)"member usage violates requested access";

std::ostream& userErrors::add(cmn::node& n, size_t id)
{
   m_errors.push_back(userError());
   auto& e = m_errors.back();
   e.initialize(n,id);
   return e.stream;
}

void userErrors::throwIfAny()
{
   if(m_errors.size()==0) return;

   std::set<userError*,errorSorter> toPrint;
   for(auto it=m_errors.begin();it!=m_errors.end();++it)
      toPrint.insert(&*it);

   std::stringstream msg;
   msg << m_errors.size() << " error(s) detected" << std::endl;
   size_t last=0;
   for(auto it=toPrint.begin();it!=toPrint.end();++it)
   {
      if(last != (*it)->id)
         msg << (const char*)((*it)->id) << std::endl;
      last = (*it)->id;
      msg << "   " << (*it)->pNode->getAncestor<cmn::fileNode>().fullPath << ", line " << (*it)->pNode->lineNumber << std::endl;
      if((*it)->stream.str().length())
         msg << "      " << (*it)->stream.str().c_str() << std::endl;
   }

   cdwTHROW(msg.str());
}

timedGlobal<userErrors> gUserErrors;

} // namespace cmn
