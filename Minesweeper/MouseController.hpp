#pragma once

#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <cmath>
#include <utility>

class Mouse {
  public:
    static constexpr NormalDuration DEFAULT_CLICK_WAIT = NormalDuration(std::chrono::milliseconds(30),
									std::chrono::milliseconds(5));
    static constexpr NormalDuration DEFAULT_MOVE_TIME = NormalDuration(std::chrono::milliseconds(200),
								       std::chrono::milliseconds(50));
    static constexpr NormalDuration DEFAULT_WAIT_AFTER_MOVE = NormalDuration(std::chrono::milliseconds(50),
									     std::chrono::milliseconds(5));
  public:
    enum class Moved : B { NO = 0, YES = 1 };
    using Coordinate = int; // Apple rounds to int now? Used to be double.
    struct Position {
	Coordinate x;
	Coordinate y;
	Position(double dx, double dy)
	    : x(static_cast<Coordinate>(std::round(dx)))
	    , y(static_cast<Coordinate>(std::round(dy))) {
	}
	Position(Coordinate ix, Coordinate iy)
	    : x(ix)
	    , y(iy) {
	}
	friend inline bool operator==(Position const& a, Position const& b) {
	    auto eps = 0.01;
	    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps;
	}
	friend inline bool operator!=(Position const& a, Position const& b) {
	    return !(a == b);
	}
    };
    
  public:
    static Position position();
    static void immediateMoveWithoutEvent(Position const& position);
    static void immediateMoveWithEvent(Position const& position);
    // returns early if mouse is manually moved
    static Moved realisticMove(Position const& position,
			       NormalDuration const& time = DEFAULT_MOVE_TIME,
			       NormalDuration const& waitAfter = DEFAULT_WAIT_AFTER_MOVE);
    // finishes the click even if the mouse is manually moved
    static Moved leftClick(NormalDuration const& timeDown = DEFAULT_CLICK_WAIT,
			   NormalDuration const& timeAfter = DEFAULT_CLICK_WAIT);
    static Moved rightClick(NormalDuration const& timeDown = DEFAULT_CLICK_WAIT,
			    NormalDuration const& timeAfter = DEFAULT_CLICK_WAIT);
    // doesn't finish the click if the mouse is manually moved
    static Moved cancellableLeftClick(NormalDuration const& timeDown = DEFAULT_CLICK_WAIT,
				      NormalDuration const& timeAfter = DEFAULT_CLICK_WAIT);
    static Moved cancellableRightClick(NormalDuration const& timeDown = DEFAULT_CLICK_WAIT,
				       NormalDuration const& timeAfter = DEFAULT_CLICK_WAIT);
    // returns early if mouse is manually moved (but finishes click if started)
    static Moved realisticMoveAndLeftClick(Position const& position,
					   NormalDuration const& moveTime = DEFAULT_MOVE_TIME,
					   NormalDuration const& waitAfterMove = DEFAULT_WAIT_AFTER_MOVE,
					   NormalDuration const& clickTimeDown = DEFAULT_CLICK_WAIT,
					   NormalDuration const& clickTimeAfter = DEFAULT_CLICK_WAIT);
};
