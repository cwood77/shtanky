#pragma once

namespace shtemu {

// TODO should really use cmn::appfmt::header in cmd/app-fmt.hpp

enum {
   kUnset            = 0xFFFFFFFF,

   kEntrypointOffset = 11+4+1,
   kOsCallOffset     = kEntrypointOffset + 4,
   kFlagsOffset      = kOsCallOffset + 4,
};

} // namespace shtemu
