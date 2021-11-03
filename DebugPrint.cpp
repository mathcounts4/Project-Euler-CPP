#include "DebugPrint.hpp"

namespace debug {

    // ALWAYS_FORCE_REBUILD
#ifdef DEBUG
    const bool doPrint = true;
#else
    const bool doPrint = false;
#endif

} /* namespace debug */
