#pragma once

namespace araceli {

class scopeNode;

class loader {
public:
   static void loadFolder(scopeNode& s);

private:
   loader();
   loader(const loader&);
   loader& operator=(const loader&);
};

} // namespace araceli
