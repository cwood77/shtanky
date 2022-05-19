#include "../cmn/trace.hpp"
#include "osCall.hpp"

namespace shtemu {

class printService : public iOsCallService {
public:
   virtual void invoke(size_t i, void *payload)
   {
      cdwINFO("[printService] '%s'\r\n",(const char *)payload);
   }
};

namespace {

osCallServiceRegistrar<printService,1> _reg;

} // anonymous namespace

} // namespace shtemu
