template<class T>
inline T& timedGlobal<T>::demand()
{
   if(!m_pPtr)
      throw std::runtime_error(cmn::fmt("timedGlobal is '%s' not active when needed",typeid(T).name()));
   return *m_pPtr;
}

template<class T>
inline const T& timedGlobal<T>::demand() const
{
   return const_cast<timedGlobal<T>*>(this)->demand();
}