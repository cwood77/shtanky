#pragma once

namespace cmn { class araceliProjectNode; }
namespace cmn { class classNode; }
namespace cmn { class scopeNode; }

namespace araceli {

class metadata;

class consoleAppTarget {
public:
   void codegen(cmn::araceliProjectNode& root, metadata& md);

private:
   cmn::scopeNode& findProjectScope(cmn::araceliProjectNode& root);
   bool wantsStream(cmn::classNode& n);
};

} // namespace araceli
