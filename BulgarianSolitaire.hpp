#pragma once

#include "Class.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

#include <list>
#include <map>

struct LenAndNumPiles {
    UI fLengthPlusIndex;
    UI fNumPiles;
};

/*

https://en.wikipedia.org/wiki/Bulgarian_solitaire

We start with a set of piles of some object.
We "shuffle" these piles by repeating the following step:
* Take 1 item from each nonempty pile, and create 1 new pile with all of those items

We observe the sequence of numbers formed by the sizes of the new piles.

Claim: the sequence of set of piles has a fixed point of: [0, 1, 2, ..., k] for some k, with added 1's on top that rotate around:

1 1 3 = 1 0 1 on top of 0 1 2
0 2 3 = 0 1 1 on top of 0 1 2
1 2 2 = 1 1 0 on top of 0 1 2
1 1 3

0 1 3 = 0 0 1 on top of 0 1 2
0 2 2 = 0 1 0 on top of 0 1 2
1 1 2 = 1 0 0 on top of 0 1 2
0 1 3

We can see that this maintains the underlying [0, 1, .., k] and shuffles around the 1's on top:
1 on top of x shuffles to 1 on top of x-1 if x > 1, simply by that same pile being preserved with 1 less.
For the new number, it equals k+1 if there is a 1 on top of 0, or k if there isn't.
Thus a 1 on top of 0 shuffles around to k.

Next, how do we know that any initial list will always converges to a sequence like this?
Next, how can we detect this kind of sequence, in order to efficiently evaluate large terms?

For convergence, we visualize the piles as a downward pointing triangle, where each diagonal has length corresponding to a pile size, and the piles are sorted largest on bottom.
For example, 1 2 4 with the diagonals going up-right would be:
     X
X X X
 X X
  X
And 2 3 5 would be:
      X
 X X X
X X X
 X X
  X
This offers a canonical view of the piles, which satisfies that any X has an X in both positions below it (excluding falling off the edges).

One step of shuffling takes the bottom up-left diagonal and makes it a new up-right diagonal.
For example, 1 2 4 -> 1 3 3:
   X
X X X
 X X
  X
And 2 3 5 -> 1 2 3 4:
X X X X
 X X X
  X X
   X

We observe that this shuffle step can be deconstructed into 3 mini-steps:
A. Flip the triangle left-right starting at the position just up-right of the bottom
B. Flip the triangle left-right starting at the bottom
C. For the up-right diagonal now at the bottom, while it is shorter than the diagonal above it, swap it with the one above it
For 1 2 4 -> 1 3 3:
     X       X             X           X
X X X   A   X X X   B   X X X   C   X X X
 X X   -->   X X   -->   X X   -->   X X
  X           X           X           X
For 2 3 5 -> 1 2 3 4:
      X       X               X
 X X X         X X X     X X X       X X X X
X X X   A     X X X   B   X X X   C   X X X
 X X   -->     X X   -->   X X   -->   X X
  X             X           X           X

We add a metric to this view of piles:
X in bottom row = 1
X in 2nd row from bottom = 2
X in nth row from bottom = n
And the metric is the sum of the values of all the X's.

For 1 2 4 -> 1 3 3:
     4       4             4           4
3 3 3   A   3 3 3   B   3 3 3   C   3 3 3
 2 2   -->   2 2   -->   2 2   -->   2 2
  1           1           1           1
For 2 3 5 -> 1 2 3 4:
      X       4               5
 4 4 4         4 4 4     4 4 4       4 4 4 4
3 3 3   A     3 3 3   B   3 3 3   C   3 3 3
 2 2   -->     2 2   -->   2 2   -->   2 2
  1             1           1           1

We see that shuffling steps (1) and (2) do not change the metric value, while (3) either reduces it (if the diagonal is swapped), or maintains it (if not swapped).
Thus this metric is monotonically decreasing as we shuffle the piles.

We say the piles are "unstable" if there exists an X in one row and a space in the row below it, and "stable" = "not unstable".
We show that the piles that are physically "stable" are in cycles, and those that are not "stable" are not in cycles.

First, we show the non-stable piles decay (by decreasing the metric), thus cannot ever come back to the initial configuration, thus are not in cycles.
Every non-stable pile has a row with an empty spot, and the row above it has an X in some spot. Let n be the number of spots in the lower of these 2 rows.
Let's assume, for contradiction, that the piles never decay.
Then, we limit our observation to just those 2 rows, which start out something like:
X X _ X _ _ _
 X X X X _ X
We show that, if the piles never decay, we result in an invalid state.
Let's consider the result of one shuffle on one row with k elements, initially 1...k:
(1) flips 2...k so we have 1, k, k-1, ..., 3, 2
(2) flips 1...2 so we have 2, 3, ..., k-1, k, 1
(3) never does anything, since we assumed that piles never decay

So 1 shuffle rotates the row to the left by 1.
But now, what happens after n+1 shuffles?
The top row has n+1 spots, so it rotates n+1 times, doing nothing.
The bottom row has n spots, so it rotates n+1 times, which is equivalent to rotating once.

Thus we can repeat these (n+1) shuffles, rotating the bottom row one spot at a time, until the empty spot is underneath an X in the top row.
This configuration is invalid since the piles are only valid when every X is sitting on top of other X's, so we reach a contradiction.

Next, we show the stable piles are in cycles.
A stable pile cannot decay, as the metric is minimal for that number of items.
Thus, above all the complete rows, there is exactly one incomplete (possibly empty) top row.
We saw that 1 shuffle rotates each row by 1, so the complete rows remain complete, and the top row rotates around.
If the top row has length n, after n steps (and also possibly fewer), it will be in the same state.
Thus a stable pile is in a cycle.


Next step: efficiently detecting stable piles.

If we can measure the metric on the list of piles, we can determine when the piles become stable.

Efficiently shuffling the piles and keeping track of the metric:
* At each step, all existing piles decrease by 1. Rather than modifying each pile, keep a counter of "how many each pile has been reduced by"
* To compute the metric decrease due to a shuffle, we consider the piles in the triangle formation:
  * The metric decreases in step (3), where we swap the bottom diagonal up until it is >= the diagonal above it.
  * Each time we swap this diagonal with the one above it, the metric decreases by the difference in their lengths

With this strategy, we can efficiently keep track of the metric as we shuffle, easily detecting when we've reached a stable state.

*/
struct BulgarianSolitaire {
  private:
    std::map<UI, UI, std::greater<UI>> fCountToNumElements;

  public:
    struct Summary {
	// sequence starting at fMinIndex (1-based) = ...fInitialSequence... ...fBaseValue+fRepeatingSequence... ...fBaseValue+fRepeatingSequence... ...fBaseValue+fRepeatingSequence... ...
	// For example, a starting sequence of piles {5,1,1} produces:
	// 3 -> {4,3}
	// 2 -> {3,2,2}
	// 3 -> {2,1,1,3}
	// 4 -> {1,2,4}
	// 3 -> {1,3,3}
	// 3 -> {2,2,3}
	// at which point we cycle, so the sequence for {5,1,1} is:
	// {3,2,[3,4,3,3]*}
	// So for starting piles {5,1,1} and minIndex = 2:
	// fMinIndex = 2, fInitialSequence = {2}, fBaseValue = 3, fRepeatingSequence = {0,1,0,0}
	UL fMinIndex;
	Vec<UI> fInitialSequence;

	UI fBaseValueForRepeatingSequence;
	Vec<B> fRepeatingSequence;

	Summary(UL minIndex, Vec<UI> fullSequence);
	Summary(UL minIndex, Vec<UI> initSequence, UI baseValueForRepeatingSequence, std::list<LenAndNumPiles> const& piles, UI pilesIndex);
	UI at(UL m) const;
    };

  public:
    void addElementWithCount(UI count);
    Summary summarize(UL min, UL max) const;
    auto const& getCountToNumElements() const {
	return fCountToNumElements;
    }
};
template<>
struct Class<BulgarianSolitaire> {
    using T = BulgarianSolitaire;
    using Proxy = std::multiset<UI, std::greater<UI>>;
    static std::ostream& print(std::ostream& os, T const& t);
    static Optional<T> parse(std::istream&);
    static std::string name();
    static std::string format();
};
template<>
struct Class<BulgarianSolitaire::Summary> {
    using T = BulgarianSolitaire::Summary;
    static std::ostream& print(std::ostream& os, T const& t);
    static Optional<T> parse(std::istream&);
    static std::string name();
    static std::string format();
};

