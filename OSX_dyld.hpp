#pragma once

#ifdef __APPLE__
#include <mach-o/dyld.h> // for _dyld_get_image_header
#else
#error "Unsupported platform"
#endif
