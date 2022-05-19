#pragma once
#include <map>
#include <stddef.h>

namespace shtemu {

class iOsCallService {
public:
   virtual void invoke(size_t i, void *payload) = 0;
};

class osCallThunk {
public:
   static osCallThunk& get();
   static void thunk(size_t i, void *payload) { get().invoke(i,payload); }

   void registerService(size_t i, iOsCallService& service);

   void invoke(size_t i, void *payload);

private:
   std::map<size_t,iOsCallService*> m_dict;
};

template<class T, size_t I>
class osCallServiceRegistrar {
public:
   osCallServiceRegistrar()
   {
      osCallThunk::get().registerService(I,m_service);
   }

private:
   T m_service;
};

} // namespace shtemu
