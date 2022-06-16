#pragma once
#include "global.hpp"
#include <map>
#include <typeinfo>
#include <vector>

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
namespace tgt { class iTargetInfo; }

namespace type {

class iType {
public:
   virtual ~iType() {}

   virtual const std::string& getName() const = 0;

   virtual const size_t getRealAllocSize(const tgt::iTargetInfo& t) const = 0;
   virtual const size_t getPseudoRefSize() const = 0;

   template<class T> bool is() const { return _is(typeid(T).name()); }
   template<class T> T& as() { return *reinterpret_cast<T*>(_as(typeid(T).name())); }
   template<class T> const T& as() const { return const_cast<iType*>(this)->as<T>(); }

   virtual bool _is(const std::string& name) const = 0;
   virtual void *_as(const std::string& name) = 0;
};

class iStructType {
public:
   virtual iType& getField(const std::string& name) = 0;
   virtual size_t getOffsetOfField(const std::string& name, const tgt::iTargetInfo& t) const = 0;
   virtual bool hasMethod(const std::string& name) const = 0;
};

class iFunctionType {
public:
   virtual iType& getReturnType() = 0;
   virtual std::vector<iType*> getArgTypes() = 0;

   virtual bool isStatic() const = 0;
   virtual iType *getClassType() = 0;
};

class table {
public:
   ~table();

   // always brings back something: a stub if the type doesn't exist yet
   iType& fetch(const std::string& name);

   // makes sure only one instance of any given type is maintained
   iType& publish(iType *pType);

   void dump();

private:
   std::map<std::string,iType*> m_allTypes;
};

extern timedGlobal<table> gTable;

class typeBuilder {
public:
   static typeBuilder *createString();
   static typeBuilder *createBool();
   static typeBuilder *createInt();
   static typeBuilder *createVoid();
   static typeBuilder *createClass(const std::string& name);
   static typeBuilder *createPtr();
   static typeBuilder *open(iType& t);
   static typeBuilder *createFunction(const std::string& fqn);

   ~typeBuilder();

   typeBuilder& wrapArray();
   typeBuilder& unwrapArray();

   // classes
   typeBuilder& addBase(iType& ty);
   typeBuilder& addMember(const std::string& name, iType& ty);
   typeBuilder& addMethod(const std::string& name);

   // function/methods
   typeBuilder& setClassType(iType& ty);
   typeBuilder& setStatic(bool v = true);
   typeBuilder& setReturnType(iType& ty);
   typeBuilder& appendArgType(const std::string& name, iType& ty);

   iType& finish();

private:
   explicit typeBuilder(iType *pT, bool own = true) : m_pType(pT), m_own(own) {}

   iType *m_pType;
   bool m_own;
};

class nodeCache {
public:
   bool hasType(const node& n) const;
   iType& demand(const node& n);
   void publish(const node& n, iType& t);

   void dump();

private:
   std::map<const node*,iType*> m_cache;
};

extern timedGlobal<nodeCache> gNodeCache;

} // namespace type
} // namespace cmn
