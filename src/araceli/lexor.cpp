#include "lexor.hpp"

namespace araceli {

static const size_t unsupported[] = {
   cmn::commonLexor::kRef,
   cmn::commonLexor::kFunc,
   0, // end
};

lexor::lexor(const char *buffer)
: commonLexor(buffer,unsupported)
{
   advance();
}

} // namespace araceli
