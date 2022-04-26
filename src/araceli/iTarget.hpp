#pragma once

namespace cmn { class araceliProjectNode; }
namespace cmn { class outStream; }

namespace araceli {

class metadata;

class iTarget {
public:
   enum phase {
      kLiamPhase,
      kShtasmPhase,
      kShlinkPhase,
   };

   virtual void addScopes(cmn::araceliProjectNode& root) = 0;
   virtual void araceliCodegen(cmn::araceliProjectNode& root, metadata& md) = 0;
   virtual void liamCodegen(cmn::outStream& sourceStream) = 0;
   virtual void adjustFiles(phase p, std::list<std::string>& files) = 0;
};

} // namespace araceli
