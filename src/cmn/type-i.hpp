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
   virtual const size_t getRealAllocSize(const tgt::iTargetInfo& t) const;
   virtual const size_t getPseudoRefSize() const { return m_size; }

protected:
   staticallySizedType(const std::string& name, size_t s) : typeBase(name), m_size(s) {}

private:
   const size_t m_size;
};

// ----------------------- primitive types -----------------------

class stringType : public staticallySizedType {
public:
   stringType() : staticallySizedType("string",0) {}
};

class intType : public staticallySizedType {
public:
   intType() : staticallySizedType("int",0) {}
};

class voidType : public staticallySizedType {
public:
   voidType() : staticallySizedType("void",0) {}
};

class ptrType : public staticallySizedType {
public:
   ptrType() : staticallySizedType("ptr",0) {}
};

// ----------------------- modifier types -----------------------

class arrayOfType : public staticallySizedType {
public:
   explicit arrayOfType(iType& inner)
   : staticallySizedType(inner.getName()+"[]",0), m_inner(inner) {}

private:
   iType& m_inner;
};

// ----------------------- user types -----------------------

class userClassType : public typeBase, public iStructType {
public:
   explicit userClassType(const std::string& name) : typeBase(name) {}

   virtual const size_t getRealAllocSize(const tgt::iTargetInfo& t) const;
   virtual const size_t getPseudoRefSize() const { return 0; }
   virtual bool _is(const std::string& name) const;
   virtual void *_as(const std::string& name);
   virtual iType& getField(const std::string& name);
   virtual size_t getOffsetOfField(const std::string& name, const tgt::iTargetInfo& t) const;

   void addField(const std::string& name, iType& f);

private:
   std::list<std::string> m_order;
   std::map<std::string,iType*> m_members;
};

// ----------------------- function type -----------------------

class functionType : public staticallySizedType, public iFunctionType {
public:
   explicit functionType(const std::string& name)
   : staticallySizedType(name,0), m_pClass(NULL), m_static(false), m_pRType(NULL) {}

   virtual bool _is(const std::string& name) const;
   virtual void *_as(const std::string& name);
   virtual iType& getReturnType() { return *m_pRType; }
   virtual std::vector<iType*> getArgTypes() { return m_argTypes; }
   virtual bool isStatic() const { return m_static; }
   virtual iType *getClassType() { return m_pClass; }

   void setClassType(iType& ty) { m_pClass = &ty; }
   void setStatic(bool v = true) { m_static = v; }
   void setReturnType(iType& ty) { m_pRType = &ty; }
   void appendArgType(const std::string& name, iType& ty);

private:
   iType *m_pClass;
   bool m_static;
   iType *m_pRType;
   std::vector<std::string> m_argNames;
   std::vector<iType*> m_argTypes;
};

// ----------------------- misc types -----------------------

class stubTypeWrapper : public iType {
public:
   explicit stubTypeWrapper(const std::string& name) : pReal(NULL), m_name(name) {}
   ~stubTypeWrapper() { delete pReal; }

   virtual const std::string& getName() const { return m_name; }
   virtual const size_t getRealAllocSize(const tgt::iTargetInfo& t) const
   { return demandReal().getRealAllocSize(t); }
   virtual const size_t getPseudoRefSize() const
   { return demandReal().getPseudoRefSize(); }
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
