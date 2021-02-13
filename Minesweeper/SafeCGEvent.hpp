#pragma once

#include "OSX_ApplicationServices.hpp"

class SafeCGEvent {
  public:
    SafeCGEvent(CGEventRef ref);
    ~SafeCGEvent() {
	CFRelease(fCGEventRef);
    }
    void trigger();
    operator CGEventRef() {
	return fCGEventRef;
    }
    CGEventRef fCGEventRef;
};

class NullCGEvent : public SafeCGEvent {
  public:
    NullCGEvent()
	: SafeCGEvent(CGEventCreate(nullptr)) {
    }
};

