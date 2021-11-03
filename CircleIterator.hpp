#pragma once

#include "Point.hpp"
#include "TypeUtils.hpp"

using CirclePoint = Point2D<SI>;

// goes from (r,0) counterclockwise
// listing the maximum point in/on the circle for each x value
// (r,0) -> (0,r) -> (-r,0) -> (0,-r) -> (r,0) (non-inclusive on final point)
class CircleIteratorX {
  private:
    UI m_r;
    class Iterator {
      private:
	UI x;
	UI y;
	UI progressNeededForNextY;
	UI progressToNextY;
	UI progressFromNextX;
	UI phase;
	friend class CircleIteratorX;
	Iterator(UI r);
      public:
	struct End {};
	CirclePoint operator*() const;
	Iterator& operator++();
	bool operator!=(End const& o) const;
    };
  public:
    CircleIteratorX(UI r);
    Iterator begin() const;
    Iterator::End end() const;
};

// lists pairs of points {in/on the circle, outside the circle}
// goes from {(r,0),(r+1,0)} counterclockwise
// (r,0) -> (0,r) -> (-r,0) -> (0,-r) -> (r,0)
// (non-inclusive on final pair equal to starting pair)
class CircleIterator {
  private:
    UI m_r;
    class Iterator {
      private:
	// {inner,outer}
	Point2D<UI> inner;
	UI breathing_room;
	UC phase;
	enum : UC { Horizontal = 0, Vertical } outer;
	void inc_y();
	void dec_x();
	
	friend class CircleIterator;
	Iterator(UI R);
      public:
	struct End {};
	// {inner,outer}
	std::pair<CirclePoint,CirclePoint> operator*() const;
	Iterator& operator++();
	bool operator!=(End const&) const;
    };
  public:
    CircleIterator(UI r);
    Iterator begin() const;
    Iterator::End end() const;
};

