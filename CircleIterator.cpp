#include "CircleIterator.hpp"

#include "ExitUtils.hpp"

#include <stdexcept>

/* CircleIteratorX */
CircleIteratorX::CircleIteratorX(UI r)
    : m_r(r) {
}

CircleIteratorX::Iterator CircleIteratorX::begin() const {
    return m_r;
}

CircleIteratorX::Iterator::End CircleIteratorX::end() const {
    return {};
}

/* CircleIteratorX::Iterator */
CircleIteratorX::Iterator::Iterator(UI r)
    : x(r)
    , y(0)
    , progressNeededForNextY(1)
    , progressToNextY(0)
    , progressFromNextX(r ? 2*r-1 : 0)
    , phase(0) {
}

Point2D<SI> CircleIteratorX::Iterator::operator*() const {
    SI const x_ = static_cast<SI>(x);
    SI y_ = static_cast<SI>(y);
    if ((phase & 1U) && progressToNextY > 0)
	--y_;
    switch (phase) {
      case 0:
	return {x_,y_};
      case 1:
	return {-x_,y_};
      case 2:
	return {-x_,-y_};
      case 3:
	return {x_,-y_};
      default:
	throw_exception<std::range_error>(std::string(__PRETTY_FUNCTION__) + ": impossible case with phase " + to_string(phase));
    }
}

CircleIteratorX::Iterator& CircleIteratorX::Iterator::operator++() {
    if (x == 0 && y == 0) {
	phase = 4;
	return *this;
    }
    
    bool const forward = (phase & 1U) == 0;
    progressToNextY += progressFromNextX;
    forward ? progressFromNextX -= 2 : progressFromNextX += 2;
    forward ? --x : ++x;
    
    while (progressToNextY >= progressNeededForNextY) {
	progressToNextY -= progressNeededForNextY;
	forward ? progressNeededForNextY += 2 : progressNeededForNextY -= 2;
	forward ? ++y : --y;
    }
    if (x == 0 || y == 0) {
	// reset and increment phase
	// (r,0) -> (0,r) -> (r,0) -> (0,r) -> (r,0)
	forward ? progressNeededForNextY -= 2 : progressNeededForNextY += 2;
	forward ? progressFromNextX += 2 : progressFromNextX -= 2;
	++phase;
    }
    return *this;
}

bool CircleIteratorX::Iterator::operator!=(CircleIteratorX::Iterator::End const &) const {
    return phase < 4;
}

/* CircleIterator */
CircleIterator::CircleIterator(UI r)
    : m_r(r) {
}

CircleIterator::Iterator CircleIterator::begin() const {
    return m_r;
}

CircleIterator::Iterator::End CircleIterator::end() const {
    return {};
}

/* CircleIterator::Iterator */
CircleIterator::Iterator::Iterator(UI R)
    : inner{R,0}
    , breathing_room(0)
    , phase(0)
    , outer(Horizontal) {
}

std::pair<CirclePoint,CirclePoint> CircleIterator::Iterator::operator*() const {
    SI const in_x = static_cast<SI>(inner.x);
    SI const in_y = static_cast<SI>(inner.y);
    SI const out_x = in_x + (outer == Horizontal);
    SI const out_y = in_y + (outer == Vertical);
    switch (phase) {
      case 0:
	return {{in_x,in_y},{out_x,out_y}};
      case 1:
	return {{-in_y,in_x},{-out_y,out_x}};
      case 2:
	return {{-in_x,-in_y},{-out_x,-out_y}};
      case 3:
	return {{in_y,-in_x},{out_y,-out_x}};
      default:
	throw_exception<std::range_error>(std::string(__PRETTY_FUNCTION__) + ": invalid case with phase " + to_string(phase));
    }
}

void CircleIterator::Iterator::inc_y() {
    breathing_room -= 2*inner.y+1;
    ++inner.y;
}

void CircleIterator::Iterator::dec_x() {
    breathing_room += 2*inner.x-1;
    --inner.x;
}

CircleIterator::Iterator& CircleIterator::Iterator::operator++() {
    if (outer == Horizontal) {
	// io (horizontal barrier)
	if (breathing_room >= inner.y*2+1) {
	    // above i is still inside:
	    // IO (new)
	    // io (old)
	    inc_y();
	} else {
	    // above i is not inside:
	    // O
	    // Io (i = I)
	    outer = Vertical;
	}
    } else {
	// o
	// i (vertical barrier)
	if (breathing_room+2*inner.x-1 >= 2*inner.y+1) {
	    // left of o is inside
	    // IO (o = O)
	    //  i
	    dec_x();
	    inc_y();
	    outer = Horizontal;
	} else {
	    // Left of o is outside
	    // Oo
	    // Ii
	    dec_x();
	}
    }
    if (inner.x == 0 && outer == Vertical) {
	++phase;
	inner.x = inner.y;
	inner.y = 0;
	outer = Horizontal;
    }
    return *this;	    
}

bool CircleIterator::Iterator::operator!=(End const &) const {
    return phase < 4;
}

