#include "../cmn/symbolTable.hpp"

namespace araceli {

class unloadedScopeFinder : public cmn::araceliVisitor<> {
public:
   explicit unloadedScopeFinder(const std::string& missingRef);

   bool any();

   // the longest, unloaded scope whose name is a logical prefix of the missing ref
   cmn::scopeNode& mostLikely();

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::scopeNode& n);

private:
   std::string m_missingRef;
   std::map<std::string,cmn::scopeNode*> m_candidates;
};

class nodeLinker : public cmn::nodeLinker {
protected:
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace araceli
