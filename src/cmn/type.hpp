#pragma once
#include "global.hpp"
#include <map>

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

   template<class T>
   bool is() const;
   template<class T>
   T& as();
   template<class T>
   const T& as() const;

protected:
   virtual bool _is(const std::string& name) const = 0;
   virtual void *_as(const std::string& name) = 0;
};

class iStructType {
public:
   virtual const iType& getField(const std::string& name) const = 0;
   virtual size_t getOffsetOfField(const std::string& name) const = 0;
};

// does the table need to be public (i.e. not type-i) ?
// YES: e.g. a class A has a field of class B and I'm type-building A
//      or could that go through the nodeCache? ...seems like it should?
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
   explicit typeBuilder(iType *pT) : m_pHead(pT) {}

   iType *m_pHead;
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
