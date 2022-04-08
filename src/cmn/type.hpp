#pragma once
#include "global.hpp"
#include <map>
#include <typeinfo>

// example needs
// - offsets in field access codegen
//   - measure (field+size) struct decls in lh files
//   - lookup (nested fields)
//   - varRef nodes can create
//     - really, it's the decl (e.g. arg) that creates
//       (or maybe the type?)
//   - fieldAccessNodes can publish parts of a type

namespace cmn {

class node;

namespace type {

class iType {
public:
   virtual ~iType() {}

   virtual const std::string& getName() const = 0;
   virtual const size_t getSize() = 0;

   template<class T> bool is() const { return _is(typeid(T).name()); }
   template<class T> T& as() { return *reinterpret_cast<T*>(_as(typeid(T).name())); }
   template<class T> const T& as() const { return const_cast<iType*>(this)->as<T>(); }

   virtual bool _is(const std::string& name) const = 0;
   virtual void *_as(const std::string& name) = 0;
};

class iStructType {
public:
   virtual const iType& getField(const std::string& name) const = 0;
   virtual size_t getOffsetOfField(const std::string& name) const = 0;
};

class table {
public:
   ~table();

   iType& fetch(const std::string& name);
   iType& publish(iType *pType);

private:
   std::map<std::string,iType*> m_allTypes;
};

timedGlobal<table> gTable;

class typeBuilder {
public:
   static typeBuilder *createString();
   static typeBuilder *createVoid();
   static typeBuilder *createClass(const std::string& name);
   static typeBuilder *createPtr();

   ~typeBuilder();

   typeBuilder& array();
   typeBuilder& addMember(const std::string& name, iType& ty);
   iType& finish();

private:
   explicit typeBuilder(iType *pT) : m_pType(pT) {}

   iType *m_pType;
};

class nodeCache {
public:
   bool hasType(const node& n) const;
   iType& demand(const node& n);
   void publish(const node& n, iType& t);
};

timedGlobal<nodeCache> gNodeCache;

} // namespace type
} // namespace cmn
