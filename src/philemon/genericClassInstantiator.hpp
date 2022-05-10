#pragma once
#include "../cmn/ast.hpp"
#include <map>

namespace philemon {

class typeParamedNameResolver : public cmn::hNodeVisitor {
public:
   explicit typeParamedNameResolver(std::map<std::string,std::string>& dict)
   : m_dict(dict) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::callNode& n);

   virtual void _implementLanguage() {} // all

   void finish();

private:
   void resolveText(std::string& s);
   void registerReplacement(cmn::node& old, cmn::node& nu);

   std::map<std::string,std::string>& m_dict;
   std::list<std::pair<cmn::node*,cmn::node*> > m_replaces;
};

class instantiationFinder : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);

   virtual void _implementLanguage() {} // all

   std::map<std::string,cmn::genericNode*> genericsByName;
   std::set<cmn::instantiateNode*> unimpled;

   void dump();
};

class classInstantiator {
public:
   size_t run(cmn::node& n);

private:
   void makeInstance(cmn::instantiateNode& n);

   void buildBinding(std::list<std::string>& args,
      cmn::genericNode& n, std::map<std::string,std::string>& dict);

   // e.g. list<T>.  The 'pseudo' refers to the fact that T is unbound
   static std::string buildInstancePseudoName(const std::string& base,
      const std::list<std::string>& args);

   std::map<std::string,cmn::genericNode*> m_genericsByName;
};

class genericStripper : public cmn::hNodeVisitor {
public:
   genericStripper() : nRemoved(0) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);

   virtual void _implementLanguage() {} // all

   size_t nRemoved;
};

} // namespace philemon
