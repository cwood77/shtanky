#pragma once
#include "iTarget.hpp"

namespace cmn { class classNode; }
namespace cmn { class scopeNode; }

namespace araceli {

class consoleAppTarget : public iTarget {
public:
   virtual void addScopes(cmn::araceliProjectNode& root);
   virtual void araceliCodegen(cmn::araceliProjectNode& root, metadata& md);
   virtual void liamCodegen(cmn::outStream& sourceStream);
   virtual void adjustFiles(phase p, std::list<std::string>& files);

private:
   cmn::scopeNode& findProjectScope(cmn::araceliProjectNode& root);
   bool wantsStream(cmn::classNode& n);
};

} // namespace araceli
