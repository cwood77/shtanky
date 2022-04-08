#pragma once
#include "fmt.hpp"
#include <stdexcept>
#include <typeinfo>

namespace cmn {

// a global that is only valid during a certain timeframe
template<class T>
class timedGlobal {
public:
   timedGlobal() : m_pPtr(NULL) {}

   T *operator->() { return &demand(); }
   const T *operator->() const { return &demand(); }

   T& operator*() { return demand(); }
   const T& operator*() const { return demand(); }

   const T* operator&() const { return &demand(); }
   T* operator&(); // illegal!  intentionally undefined

   T& demand();
   const T& demand() const;

private:
   T *m_pPtr;
};

template<class T>
class globalPublishTo {
public:
   globalPublishTo(T& v, timedGlobal<T>& w);
   ~globalPublishTo();

private:
   T& m_v;
   timedGlobal<T>& m_w;
};

#include "global.ipp"

} // namespace cmn
