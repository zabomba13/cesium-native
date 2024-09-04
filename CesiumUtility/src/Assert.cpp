#if defined CESIUM_FORCE_ASSERTIONS && defined NDEBUG

#include <CesiumUtility/Assert.h>

#undef NDEBUG
#include <cassert>
#define NDEBUG

namespace CesiumUtility {
void forceAssertFailure() { assert(0 && "Assertion failed"); }
}; // namespace CesiumUtility

#endif
