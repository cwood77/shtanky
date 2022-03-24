#include "lexor.hpp"

namespace liam {

static const size_t unsupported[] = {
   cmn::commonLexor::kInterface,
   cmn::commonLexor::kOverride,
   cmn::commonLexor::kAbstract,
   cmn::commonLexor::kStatic,

   cmn::commonLexor::kPublic,
   cmn::commonLexor::kProtected,
   cmn::commonLexor::kPrivate,
   0, // end
};

lexor::lexor(const char *buffer)
: commonLexor(buffer,unsupported)
{
   advance();
}

} // namespace liam
