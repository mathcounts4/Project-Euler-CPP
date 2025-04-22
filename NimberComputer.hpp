#include "TypeUtils.hpp"
#include "Vec.hpp"

// https://en.wikipedia.org/wiki/Nimber
// https://en.wikipedia.org/wiki/Sprague%E2%80%93Grundy_theorem
// Example use:
/*
// Represents a Nim game with 1 pile where each player can remove anywhere from 1 to 3 stones from the pile.
// Clearly a state with i stones is winning iff i ≢ 0 mod 4, but this class will compute this.
struct NimRemoveAtMost3 : NimberComputer<NimRemoveAtMost3> {
    struct State {
        SZ fPileSize;
    };
    UI lookup(State const& s) {
        while (fNimbers.size() <= s.fPileSize) {
            fNimbers.push_back(compute(fNimbers.size()));
        }
        return fNimbers[s.fPileSize];
    }
    template<class ProcessPosition>
    void forEachPredecessor(State const& s, ProcessPosition const& processPosition) {
        for (UI i = 1; i <= 3 && i <= s.fPileSize; ++i) {
            processPosition(State{s.fPileSize - i}); // can also call processPosition(Vec<State>{...})
        }
    }
    Vec<UI> fNimbers;
};
 */
template<class Self, class NimValue = UI>
struct NimberComputer {
    Self& self() {
	return static_cast<Self&>(*this);
    }
    
    template<class State>
    NimValue compute(State const& state) {
	Vec<B> hittable;
	auto mark = [&hittable](NimValue i) {
	    if (i >= hittable.size()) {
		hittable.resize(i + 1, false);
	    }
	    hittable[i] = true;
	};
	auto processPrevPosition = [&mark, this](auto const& predecessorStateOrStateList) {
	    if constexpr (std::is_same_v<State, std::decay_t<decltype(predecessorStateOrStateList)>>) {
	        mark(self().lookup(predecessorStateOrStateList));
	    } else {
		NimValue value = 0;
		for (auto const& predecessorState : predecessorStateOrStateList) {
		    value ^= self().lookup(predecessorState);
		}
		mark(value);
	    }
	};
	self().forEachPredecessor(state, processPrevPosition);
	NimValue i = 0;
	for (; i < hittable.size(); ++i) {
	    if (!hittable[i]) {
	        break;
	    }
	}
	return i;
    }
};
