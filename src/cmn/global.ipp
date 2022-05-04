template<class T>
inline T& timedGlobal<T>::demand()
{
   if(!m_pPtr)
      cdwTHROW("timedGlobal of '%s' not active when needed",typeid(T).name());
   return *m_pPtr;
}

template<class T>
inline const T& timedGlobal<T>::demand() const
{
   return const_cast<timedGlobal<T>*>(this)->demand();
}
