#include "lexor.hpp"

namespace araceli {

static const size_t unsupported[] = {
   cmn::commonLexor::kRef,
   0, // end
};

lexor::lexor(const char *buffer)
: commonLexor(buffer,unsupported)
{
   advance();
}

} // namespace araceli
