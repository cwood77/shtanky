#include "../cmn/commonLexor.hpp"

namespace liam {

class lexor : public cmn::commonLexor {
public:
   explicit lexor(const char *buffer);
};

} // namespace liam
