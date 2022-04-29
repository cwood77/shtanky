#include "iTarget.hpp"

namespace shlink {

class shtankyAppSymbolIndex : public iSymbolIndex {
public:
   enum { kUnset = 0xFFFFFFFF };

   shtankyAppSymbolIndex() : osCallImplOffset(kUnset), entrypointOffset(kUnset) {}

   virtual void reportSymbolLocation(
      const std::string& name, unsigned long finalOffset);

   unsigned long osCallImplOffset;
   unsigned long entrypointOffset;
};

class shtankyAppTarget : public iTarget {
public:
   virtual void readArguments(cmn::cmdLine& cl);

   virtual iSymbolIndex *createIndex() { return new shtankyAppSymbolIndex(); }

   virtual void seedRequirements(layoutProgress& lProg);

   virtual void write(const layout& l, iSymbolIndex& x);

private:
   std::string m_appFilePath;
};

} // namespace shlink
