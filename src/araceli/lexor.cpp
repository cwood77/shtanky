#include "lexor.hpp"

namespace araceli {

lexor::lexor(const char *buffer)
: commonLexor(buffer,NULL)
{
   advance();
}

} // namespace araceli
