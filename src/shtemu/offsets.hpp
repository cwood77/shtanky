#pragma once

namespace shtemu {

enum {
   kEntrypointOffset = 11+4+1,
   kOsCallOffset     = kEntrypointOffset + 4,
   kFlagsOffset      = kOsCallOffset + 4,
};

} // namespace shtemu
