#pragma once
#include <list>
#include <map>
#include <set>
#include <string>

// a variable is something that requires storage (register, stack, memory, immediate).
// all args have cooresponding variables
//
// variables are created during initial codegen; they're useful then for linking args
// of different instructions together

namespace cmn { class node; }

namespace liam {

class lirArg;
class lirArgVar;
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

   void requireStorage(lirInstr& i, size_t s);

   bool isAlive(size_t orderNum);
   std::set<size_t> getStorageAt(size_t orderNum);
   bool requiresStorageLater(size_t orderNum, size_t storage);

   void updateStorageHereAndAfter(lirInstr& i, size_t old, size_t nu);
};

class varTable {
public:
   ~varTable();

   std::string pickUniqueName(const std::string& nameHint);
   var& create(const std::string& name);
   var& demand(const std::string& name);
   var& demand(lirArg& a);

   std::map<std::string,var*>& all() { return m_vars; }

   size_t getStorageFor(size_t orderNum, lirArg& a);

private:
   std::map<std::string,var*> m_vars;
};

class varWriter;

class varGenerator {
public:
   explicit varGenerator(varTable& t) : m_vTable(t) {}
   ~varGenerator();

   varWriter createPrivateVar(size_t instrNum, lirArg& a, const std::string& nameHint, ...);
   varWriter createNamedVar(size_t instrNum, lirArgVar& a);

   lirArg& claimAndAddArgByName(lirInstr& i, const std::string& n);
   lirArg& claimAndAddArgOffWire(lirInstr& i, cmn::node& n);

private:
   void publishOnWire(cmn::node& n, var& v);
   void donateToWire(cmn::node& n, lirArg& a, const std::string& nameHint);
   lirArg& duplicateArg(const lirArg& a);

   varTable& m_vTable;
   std::map<cmn::node*,var*> m_onWire;
   std::map<cmn::node*,lirArg*> m_donations;
   std::map<cmn::node*,std::string> m_donatedNames;

friend class varWriter;
};

class varWriter {
public:
   varWriter(varGenerator *pVGen, const std::string& nameHint, size_t num, lirArg *a)
   : m_pVGen(pVGen), m_nameHint(nameHint), m_num(num), m_pArg(a)
   , m_pub(false), m_donated(false) {}
   ~varWriter() { finalize(); }

   void publishOnWire(cmn::node& n)
   { m_pVGen->publishOnWire(n,createVar()); m_pub = true; }
   void donateToWire(cmn::node& n)
   { m_pVGen->donateToWire(n,*m_pArg,m_nameHint);  m_donated=true; }

private:
   var& createVar();
   void finalize();

   varGenerator *m_pVGen;
   std::string m_nameHint;
   size_t m_num;
   lirArg *m_pArg;
   bool m_pub;
   bool m_donated;
};

} // namespace liam
