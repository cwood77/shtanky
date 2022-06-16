#include "../cmn/stlutil.hpp"
#include "../cmn/trace.hpp"
#include "osCall.hpp"

namespace shtemu {

osCallThunk& osCallThunk::get()
{
   static osCallThunk thing;
   return thing;
}

void osCallThunk::registerService(size_t i, iOsCallService& service)
{
   cmn::addUnique(m_dict,i,&service);
   cdwDEBUG("service registered %lld => %s\r\n",i,typeid(service).name());
}

void osCallThunk::invoke(size_t i, void *payload)
{
   iOsCallService *pSvc = m_dict[i];
   if(!pSvc)
      cdwINFO("unknown service invoked via osCall: %lld\r\n",i);
   else
      pSvc->invoke(i,payload);
}

} // namespace shtemu
