#include "../cmn/commonLexor.hpp"

namespace araceli {

class lexor : public cmn::commonLexor {
public:
   explicit lexor(const char *buffer);
};

} // namespace araceli
