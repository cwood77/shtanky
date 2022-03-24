#pragma once

namespace cmn { class liamProjectNode; }

namespace liam {

class refNodeFinder {
};

class projectBuilder {
public:
   static void build(cmn::liamProjectNode& p);

private:
   static bool loadFileIf(cmn::liamProjectNode& p, const std::string& fullPath);
};

} // namespace liam
