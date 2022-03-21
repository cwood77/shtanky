#pragma once
#include "ast.hpp"
#include <memory>

namespace araceli {

class projectBuilder {
public:
   static std::unique_ptr<projectNode> create(const std::string& targetType);
   static void addScope(cmn::node& p, const std::string& path, bool inProject);

private:
   projectBuilder();
   projectBuilder(const projectBuilder&);
   projectBuilder& operator=(const projectBuilder&);
};

} // namespace araceli
