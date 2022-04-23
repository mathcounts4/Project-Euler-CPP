#include "DebugPrint.hpp"

namespace debug {

    // FORCE_REBUILD_DEBUG_CHANGED
#ifdef DEBUG
    const bool doPrint = true;
#else
    const bool doPrint = false;
#endif

} /* namespace debug */
