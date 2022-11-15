#include "MouseController.hpp"
#include "../Now.hpp"
#include "../Rand.hpp"
#include "../Vec.hpp"
#include "../Wait.hpp"
#include "OSX_ApplicationServices.hpp"
#include "SafeCGEvent.hpp"

#include <cmath>

class MouseMoveEvent : public SafeCGEvent {
  public:
    MouseMoveEvent(Mouse::Position const& position)
	: SafeCGEvent(CGEventCreateMouseEvent(nullptr,
					      kCGEventMouseMoved,
					      CGPointMake(position.x, position.y),
					      kCGMouseButtonLeft /* ignored */)) {
    }
};

class MouseDownEvent : public SafeCGEvent {
  public:
    static CGEventType buttonToEvent(CGMouseButton button) {
	switch (button) {
	  case kCGMouseButtonLeft:
	    return kCGEventLeftMouseDown;
	  case kCGMouseButtonRight:
	    return kCGEventRightMouseDown;
	  case kCGMouseButtonCenter:
	    return kCGEventOtherMouseDown;
	}
	return kCGEventOtherMouseDown;
    }
    MouseDownEvent(CGPoint point, CGMouseButton button = kCGMouseButtonLeft)
	: SafeCGEvent(CGEventCreateMouseEvent(nullptr,
					      buttonToEvent(button),
					      point,
					      button)) {
    }
};

class MouseUpEvent : public SafeCGEvent {
  public:
    static CGEventType buttonToEvent(CGMouseButton button) {
	switch (button) {
	  case kCGMouseButtonLeft:
	    return kCGEventLeftMouseUp;
	  case kCGMouseButtonRight:
	    return kCGEventRightMouseUp;
	  case kCGMouseButtonCenter:
	    return kCGEventOtherMouseDown;
	}
	return kCGEventOtherMouseDown;
    }
    MouseUpEvent(CGPoint point, CGMouseButton button = kCGMouseButtonLeft)
	: SafeCGEvent(CGEventCreateMouseEvent(nullptr,
					      buttonToEvent(button),
					      point,
					      button)) {
    }
};

Mouse::Position Mouse::position() {
    // ensure the mouse and cursor match before querying mouse position via an event
    CGAssociateMouseAndMouseCursorPosition(true);
    NullCGEvent event;
    CGPoint cursor = CGEventGetLocation(event);
    return {cursor.x, cursor.y};
}

void Mouse::immediateMoveWithoutEvent(Position const& position) {
    CGPoint warpPoint = CGPointMake(position.x, position.y);
    CGWarpMouseCursorPosition(warpPoint);
    // CGWarpMouseCursorPosition can disassociate the mouse and cursor for 0.25s.
    // CGAssociateMouseAndMouseCursorPosition(true) avoids this.
    CGAssociateMouseAndMouseCursorPosition(true);
}

void Mouse::immediateMoveWithEvent(Position const& position) {
    MouseMoveEvent event(position);
    event.trigger();
    CGAssociateMouseAndMouseCursorPosition(true);
}

struct PosAndTime {
    Time wait;
    Mouse::Position position;
};

static Vec<PosAndTime> intermediateMovements(Mouse::Position const& start,
					     Mouse::Position const& end,
					     UI numIntermediateMovements,
					     std::function<Time()> const& movementTimeFunction) {
    // Doesn't include the start or end points.
    // Generates a list of intermediate movements and times to wait after each movement.
    
    // Based on looking at some actual mouse movements,
    // an "efficient" human mouse move looks like a logistic curve, in both coordinates.
    // In addition, the time before reaching the final pixel is around 0.2s, regardless of distance.
    // Lastly, mouse movements are actually registered every 7.2-8.6 ms, and seems to be independent of mouse speed.
    
    // Logistic function goes from 0 (at x=-inf) to 1 (at x=inf), centered at 0.5:
    // f(x) = 1/(1 + e^(-k(x-0.5)))
    // We linearly map the result from [f(0),f(1)] -> [oldX,x] and [f(0),f(1)] -> [oldY,y]
    // Based on emperical data, k=9 is representative of mouse movements.
    auto logisticFunction = [](double x0to1) { return 1 / (1 + exp(-9 * (x0to1 - 0.5))); };
    auto logisticMin = logisticFunction(0);
    auto logisticEnd = logisticFunction(1);
    // scale the output [f(0),f(1)] -> [0,1]
    // So this now maps 0 -> 0 and 1 -> 1
    auto logisticFunction01 = [&](double x0to1) { return (logisticFunction(x0to1) - logisticMin) / (logisticEnd - logisticMin); };
    // linear scale [0,1] -> [oldX,x] and [oldY,y]
    auto logisticFunction01toX = [&](double v0to1) {
	auto logisticOutput01 = logisticFunction01(v0to1);
	return (1 - logisticOutput01) * start.x + logisticOutput01 * end.x;
    };
    auto logisticFunction01toY = [&](double v0to1) {
	auto logisticOutput01 = logisticFunction01(v0to1);
	return (1 - logisticOutput01) * start.y + logisticOutput01 * end.y;
    };
    
    UI xDelay = 0;
    UI yDelay = 0;
    if (auto delayVar = randNormal(0, numIntermediateMovements / 4.0); delayVar > 0) {
	xDelay = static_cast<UI>(floor(delayVar));
	if (xDelay >= numIntermediateMovements) {
	    xDelay = numIntermediateMovements - 1;
	}
    } else {
	yDelay = static_cast<UI>(floor(-delayVar));
	if (yDelay >= numIntermediateMovements) {
	    yDelay = numIntermediateMovements - 1;
	}
    }
    auto fractionOfWay = [](UI movement0based, UI delay, UI numDelaysAndMovements) {
	if (movement0based < delay) {
	    return 0.0;
	} else {
	    auto numMovements = numDelaysAndMovements - delay;
	    return (movement0based - delay + 1) / static_cast<D>(numMovements + 1);
	}
    };
    Vec<PosAndTime> movements;
    for (UI i = 0; i < numIntermediateMovements; ++i) {
	auto movementTime = movementTimeFunction();
	auto movementX = logisticFunction01toX(fractionOfWay(i, xDelay, numIntermediateMovements));
	auto movementY = logisticFunction01toY(fractionOfWay(i, yDelay, numIntermediateMovements));
	movements.push_back({movementTime, {movementX, movementY}});
    }
    return movements;
}

[[nodiscard]] static Mouse::Moved immediateMoveWithEventIfAtExpectedPosition(Mouse::Position const& position, Mouse::Position& expectedOldPosition) {
    auto currentPosition = Mouse::position();
    if (currentPosition != expectedOldPosition) {
	return Mouse::Moved::YES;
    }
    Mouse::immediateMoveWithEvent(position);
    expectedOldPosition = position;
    return Mouse::Moved::NO;
}

Mouse::Moved Mouse::realisticMove(Position const& endPosition,
				  NormalDuration const& timeDuration,
				  NormalDuration const& waitAfter) {
    constexpr auto mouseEventRegisterTime = 0.0079;
    constexpr auto mouseEventRegisterStdDev = 0.0005;

    auto lastPos = position();
    if (canWait()) {
	auto time = timeDuration.toDouble();
	// Even when time is dilated, do not change the time between mouse movements
	// to avoid overloading the system.
	auto selfManagedTime = NormalDuration::setSlowDownFactor(1);

	auto numMovementsWithWait = static_cast<UI>(round(time / mouseEventRegisterTime));
	auto movementTimeFunction = [&]() -> Time { return Time(randNormal(mouseEventRegisterTime, mouseEventRegisterStdDev)); };
	auto movements = intermediateMovements(lastPos, endPosition, numMovementsWithWait, movementTimeFunction);

	Time expectedDelay(0);
	UI steps = 0;
	auto before = now();
	for (auto const& [waitAfter, intermediatePosition] : movements) {
	    auto moved = immediateMoveWithEventIfAtExpectedPosition(intermediatePosition, lastPos);
	    if (moved == Moved::YES) {
		return moved;
	    }
	    auto waitAfterModified = waitAfter;
	    if (steps > 0) {
		// self-correcting wait time:
		// If the system delays more than we tell it to,
		// shorten how long we tell it.
		Time diff = timeFrom(before);
		if (diff > expectedDelay) {
		    // actual delay > expected delay
		    auto adjustment = (diff - expectedDelay) / steps;
		    waitAfterModified -= adjustment;
		}
	    }
	    waitFor(Time(waitAfterModified));
	    ++steps;
	    expectedDelay += waitAfterModified;
	}
    }
    // final movement to actually reach the point
    auto movedAtEnd = immediateMoveWithEventIfAtExpectedPosition(endPosition, lastPos);
    if (movedAtEnd == Moved::NO) {
	waitFor(waitAfter);
    }
    return movedAtEnd;
}

static Mouse::Moved click(NormalDuration const& timeDown,
			      NormalDuration const& timeAfter,
			      bool cancellable,
			      CGMouseButton button = kCGMouseButtonLeft) {
    auto startPosition = Mouse::position();
    {
	MouseDownEvent down(CGPointMake(startPosition.x, startPosition.y), button);
	down.trigger();
    }
    waitFor(timeDown);
    auto endPosition = Mouse::position();
    if (cancellable && startPosition != endPosition) {
	return Mouse::Moved::YES;
    }
    {
	// finish click
	MouseUpEvent up(CGPointMake(endPosition.x, endPosition.y), button);
	up.trigger();
    }
    waitFor(timeAfter);
    if (startPosition != endPosition) {
	return Mouse::Moved::YES;
    } else {
	return Mouse::Moved::NO;
    }
}

Mouse::Moved Mouse::leftClick(NormalDuration const& timeDown,
			      NormalDuration const& timeAfter) {
    return click(timeDown, timeAfter, false);
}

Mouse::Moved Mouse::cancellableLeftClick(NormalDuration const& timeDown,
					 NormalDuration const& timeAfter) {
    return click(timeDown, timeAfter, true);
}

Mouse::Moved Mouse::rightClick(NormalDuration const& timeDown,
			       NormalDuration const& timeAfter) {
    return click(timeDown, timeAfter, false, kCGMouseButtonRight);
}

Mouse::Moved Mouse::cancellableRightClick(NormalDuration const& timeDown,
					  NormalDuration const& timeAfter) {
    return click(timeDown, timeAfter, true, kCGMouseButtonRight);
}

Mouse::Moved Mouse::realisticMoveAndLeftClick(Position const& position,
					      NormalDuration const& moveTime,
					      NormalDuration const& waitAfterMove,
					      NormalDuration const& clickTimeDown,
					      NormalDuration const& clickTimeAfter) {
    auto moveResult = realisticMove(position, moveTime, waitAfterMove);
    if (moveResult == Moved::YES) {
	return moveResult;
    }
    return leftClick(clickTimeDown, clickTimeAfter);
}

