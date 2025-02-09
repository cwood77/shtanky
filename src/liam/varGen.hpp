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
   std::map<size_t,std::list<lirArg*> > refs;

   std::map<size_t,std::set<size_t> > instrToStorageMap;
   std::map<size_t,std::set<size_t> > storageToInstrMap;
   std::map<lirArg*,size_t> storageDisambiguators;

   const lirArg& lastArg();
   size_t getSize();

   // this unbinds the argument, but keeps the storage assignment
   void unbindArgButKeepStorage(lirInstr& i, lirArg& a);

   std::string getImmediateData() { return lastArg().getName(); }

   bool isAlive(size_t orderNum);
   bool isAlive(size_t start, size_t end);
   std::set<size_t> getStorageAt(size_t orderNum);
   size_t getStorageFor(size_t orderNum, lirArg& a);
   size_t requiresStorageNext(size_t orderNum, size_t storage);
   bool alreadyWantedStorage(size_t orderNum, size_t storage);

   void requireStorage(size_t orderNum, size_t s);
   void changeStorage(size_t orderNum, size_t old, size_t nu);
   void updateStorageHereAndAfter(lirInstr& i, size_t old, size_t nu);

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
