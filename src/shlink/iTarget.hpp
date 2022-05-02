#pragma once
#include <string>

namespace cmn { class cmdLine; }

namespace shlink {

class iSymbolIndex;
class layout;
class layoutProgress;

class iTarget {
public:
   virtual ~iTarget() {}
   virtual void readArguments(cmn::cmdLine& cl) = 0;
   virtual iSymbolIndex *createIndex() = 0;
   virtual void seedRequirements(layoutProgress& lProg) = 0;
   virtual void write(const layout& l, iSymbolIndex& x) = 0;
};

class iSymbolIndex {
public:
   virtual ~iSymbolIndex() {}
   virtual void reportSymbolLocation(
      const std::string& name, unsigned long finalOffset) = 0;
};

iTarget *createTarget(cmn::cmdLine& cl);

} // namespace shlink
