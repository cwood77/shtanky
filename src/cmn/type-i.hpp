#pragma once
#include "type.hpp"

namespace cmn {
namespace type {

class typeBase : public iType {
public:
   virtual const std::string& getName() const { return m_name; }
   virtual bool _is(const std::string& name) const;
   virtual void *_as(const std::string& name);

protected:
   typeBase(const std::string& name) : m_name(name) {}

private:
   const std::string m_name;
};

class staticallySizedType : public typeBase {
public:
   virtual const size_t getSize() { return m_size; }

protected:
   staticallySizedType(const std::string& name, size_t s) : typeBase(name), m_size(s) {}

private:
   const size_t m_size;
};

// ----------------------- primitive types -----------------------

#if 0
class stringType : public iType {
};
#endif

class voidType : public staticallySizedType {
public:
   voidType() : staticallySizedType("void",0) {}
};

#if 0
class ptrType : public iType {
};
#endif

// ----------------------- modifier types -----------------------

#if 0
class arrayOfType : public iType {
};
#endif

// ----------------------- user types -----------------------

class userClassType : public typeBase, public iStructType {
public:
   explicit userClassType(const std::string& name) : typeBase(name) {}

   virtual const size_t getSize() { throw 3.14; }
   virtual const iType& getField(const std::string& name) const { throw 3.14; }
   virtual size_t getOffsetOfField(const std::string& name) const { throw 3.14; }

   std::map<std::string,iType*> m_members;
};

// ----------------------- misc types -----------------------

class stubTypeWrapper : public iType {
public:
   explicit stubTypeWrapper(const std::string& name) : m_name(name) {}
   ~stubTypeWrapper() { delete pReal; }

   virtual const std::string& getName() const { return m_name; }
   virtual const size_t getSize() { return demandReal().getSize(); }
   virtual bool _is(const std::string& name) const { return demandReal()._is(name); }
   virtual void *_as(const std::string& name) { return demandReal()._as(name); }

   iType *pReal;

private:
   iType& demandReal();
   const iType& demandReal() const;

   const std::string m_name;
};

} // namespace type
} // namespace cmn
