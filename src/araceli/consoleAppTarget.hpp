#pragma once
#include "iTarget.hpp"

namespace cmn { class classNode; }
namespace cmn { class outStream; }
namespace cmn { class scopeNode; }

namespace araceli {

class consoleAppTarget : public iTarget {
public:
   virtual void addAraceliStandardLibrary(cmn::araceliProjectNode& root);
   virtual void populateIntrinsics(cmn::araceliProjectNode& root);
   virtual void populateInstantiates(cmn::araceliProjectNode& root);
   virtual void araceliCodegen(cmn::araceliProjectNode& root, metadata& md);
   virtual void liamCodegen(cmn::outStream& sourceStream);
   virtual void adjustBatchFileFiles(phase p, std::list<std::string>& files);

private:
   cmn::scopeNode& findProjectScope(cmn::araceliProjectNode& root);
   bool wantsStream(cmn::classNode& n);
   void callRunMethod(cmn::node& obj, size_t instNum, cmn::outStream& s);
};

} // namespace araceli
