#include "SafeCGEvent.hpp"

#include "Keyboard.hpp"

SafeCGEvent::SafeCGEvent(CGEventRef ref)
    : fCGEventRef(ref) {
    CGEventSetFlags(*this, Keyboard::currentModifierFlags());
}

void SafeCGEvent::trigger() {
    CGEventPost(kCGHIDEventTap, fCGEventRef);
}
