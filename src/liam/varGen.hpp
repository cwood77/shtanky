#pragma once
#include <list>
#include <map>
#include <string>

// a variable is something that requires storage (register, stack, memory, immediate).
// all args have cooresponding variables
//
// variables are created during initial codegen; they're useful then for linking args
// of different instructions together

namespace cmn { class node; }

namespace liam {

class lirArg;
class lirInstr;

class var {
public:
   std::string name;
   std::map<size_t,std::list<lirArg*> > refs;

   const lirArg& lastArg();
   size_t getSize();

   void requireStorage(lirInstr& i, size_t s);
};

class varTable {
public:
   ~varTable();

   std::string pickUniqueName(const std::string& nameHint);
   var& create(const std::string& name);
   var& demand(const std::string& name);
   var& demand(lirArg& a);

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
   void donateToWire(cmn::node& n, lirArg& a);
   lirArg& duplicateArg(const lirArg& a);

   varTable& m_vTable;
   std::map<cmn::node*,var*> m_onWire;
   std::map<cmn::node*,lirArg*> m_donations;

friend class varWriter;
};

class varWriter {
public:
   varWriter(varGenerator *pVGen, var *v, size_t num, lirArg *a)
   : m_pVGen(pVGen), m_pVar(v), m_num(num), m_pArg(a), m_donated(false) {}
   ~varWriter() { finalize(); }

   void publishOnWire(cmn::node& n) { m_pVGen->publishOnWire(n,*m_pVar); }
   void donateToWire(cmn::node& n) { m_pVGen->donateToWire(n,*m_pArg);  m_donated=true; }

private:
   void finalize();

   varGenerator *m_pVGen;
   var *m_pVar;
   size_t m_num;
   lirArg *m_pArg;
   bool m_donated;
};

} // namespace liam