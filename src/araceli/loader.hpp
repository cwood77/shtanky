#pragma once

namespace cmn { class scopeNode; }

namespace araceli {

class loader {
public:
   static void loadFolder(cmn::scopeNode& s);

private:
   loader();
   loader(const loader&);
   loader& operator=(const loader&);
};

} // namespace araceli
