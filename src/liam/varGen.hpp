#pragma once
#include <list>
#include <map>
#include <set>
#include <string>

namespace cmn { class node; }
namespace cmn { class outStream; }

namespace liam {

class lirArg;
class lirInstr;

class var {
public:
   std::string name;
private:
   std::map<size_t,std::list<lirArg*> > refs;
public:

   std::map<size_t,std::set<size_t> > getInstrToStorageMap() const
   { return instrToStorageMap; }

private:
   std::map<size_t,std::set<size_t> > instrToStorageMap;
   std::map<size_t,std::set<size_t> > storageToInstrMap;
public:
   std::map<lirArg*,size_t> storageDisambiguators;

   void addRef(size_t orderNum, lirArg& a);
   size_t estimatePopularity() const;

   bool hasArg(lirArg& a) const;
   const lirArg& onlyArg(size_t orderNum);
   const lirArg& lastArg();
   size_t getSize();

   // this unbinds the argument, but keeps the storage assignment
   void unbindArgButKeepStorage(lirInstr& i, lirArg& a);

   std::set<size_t> getInstrsWithStorage(size_t s) const;
   bool hasAnyStorageEver() const;

   std::string getImmediateData() { return lastArg().getName(); }

   size_t firstUsage() const;
   size_t lastUsage() const;
   bool isAlive(size_t orderNum);
   bool isAlive(size_t start, size_t end);

   std::set<size_t> getStorageAt(size_t orderNum);
   size_t getStorageFor(size_t orderNum, lirArg& a);

   // these two are used to rank usages by the combiner
      // whens the next (i.e. after 'orderNum') requirement on 'storage'?
   size_t requiresStorageNext(size_t orderNum, size_t storage);
      // was this variable's most previous storage this storage?
   bool alreadyWantedStorage(size_t orderNum, size_t storage);

   // n.b. add a new requirement, but don't change existing requirements
   void requireStorage(size_t orderNum, size_t s);
   void requireStorage(size_t orderNum, lirArg& a, size_t s);
   void changeStorage(size_t orderNum, size_t old, size_t nu);

   void format(cmn::outStream& s);
   std::string storageToString(size_t s);
};

class virtStackTable {
public:
   virtStackTable() : m_next(0) {}

   size_t reserveVirtStorage(size_t real);
   size_t mapToReal(size_t virt);

   void format(cmn::outStream& s);

private:
   std::map<size_t,size_t> m_map;
   int m_next;
};

class varTable {
public:
   ~varTable();

   var& create(const std::string& name);
   var& demand(const std::string& name);
   var& demand(lirArg& a);
   var *fetch(lirArg& a);

   std::map<std::string,var*>& all() { return m_vars; }

   virtStackTable& getVirtualStack() { return m_vSTable; }

   void format(cmn::outStream& s);

private:
   std::map<std::string,var*> m_vars;
   virtStackTable m_vSTable;
};

class varTableAutoLogger : public cmn::iLogger {
public:
   typedef varTable argType;

   explicit varTableAutoLogger(varTable& vt) : m_vt(vt) {}

   virtual std::string getExt() { return ".vars"; }
   virtual void dump(cmn::outStream& s) { m_vt.format(s); }

private:
   varTable& m_vt;
};

} // namespace liam
