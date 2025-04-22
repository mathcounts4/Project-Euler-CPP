#include "../NimberComputer.hpp"
#include "harness.hpp"
#include "../Cleanup.hpp"
#include "../MapUtils.hpp"
#include "../MyHashMultiset.hpp"
#include "../U_Map.hpp"

struct NimRemoveAtMost3 : NimberComputer<NimRemoveAtMost3> {
    struct State {
	SZ fPileSize;
    };
    UI lookup(State const& s) {
	while (fNimbers.size() <= s.fPileSize) {
	    fNimbers.push_back(compute(State{fNimbers.size()}));
	}
	return fNimbers[s.fPileSize];
    }
    template<class ProcessPosition>
    void forEachPredecessor(State const& s, ProcessPosition const& processPosition) {
	for (SZ i = 1; i <= 3 && i <= s.fPileSize; ++i) {
	    processPosition(State{s.fPileSize - i}); // can also call processPosition(Vec<State>{...})
	}
    }
    Vec<UI> fNimbers;

  public:
    UI lookup(UI x) {
	return lookup(State{x});
    }
};

TEST(NimberComputer, Basic) {
    NimRemoveAtMost3 computer;
    CHECK(computer.lookup(14596), equals(computer.lookup(0)));
    CHECK(computer.lookup(14597), equals(computer.lookup(1)));
    CHECK(computer.lookup(14598), equals(computer.lookup(2)));
    CHECK(computer.lookup(14599), equals(computer.lookup(3)));
    CHECK(computer.lookup(14600), equals(computer.lookup(0)));
}

struct NormalNim : NimberComputer<NormalNim> {
    using State = std::multiset<UI>;
    UI lookup(State const& s) {
	return getOrInsert(fNimbers, s, [&, this]() { return compute(s); });
    }
    U_Map<State, UI> fNimbers;

    struct Move {
	SZ fOldPileSize;
	SZ fNewPileSize;

	friend B operator==(Move const& x, Move const& y) {
	    return x.fOldPileSize == y.fOldPileSize && x.fNewPileSize == y.fNewPileSize;
	}
    };
    
    template<class ProcessPosition>
    std::optional<Move> forEachPredecessorImpl(State const& s, ProcessPosition const& processPosition) {
	State copy(s);
	for (auto x : s) {
	    copy.erase(copy.find(x));
	    Cleanup c{[&]() { copy.insert(x); }};

	    if (auto result = processPosition(copy)) {
		// completely remove this pile
		return Move{x, 0};
	    }
	    for (auto i = x; --i; ) {
		auto it = copy.insert(i);
		Cleanup c2{[&]() { copy.erase(it); }};
		if (auto result = processPosition(copy)) {
		    // replace pile x with i
		    return Move{x, i};
		}
	    }
	}
	return std::nullopt;
    }
    template<class ProcessPosition>
    void forEachPredecessor(State const& s, ProcessPosition const& processPosition) {
	forEachPredecessorImpl(s, [&processPosition](State const& prev) { processPosition(prev); return false; });
    }

    std::optional<Move> winningMove(State const& s) {
	return forEachPredecessorImpl(s, [this](State const& prev) { return lookup(prev) == 0; });
    }
};

TEST(NimberComputer, FindWinningMove) {
    NormalNim computer;
    CHECK((computer.winningMove({3, 4}) == NormalNim::Move{4, 3}), isTrue());
    CHECK((computer.winningMove({2, 3, 7, 11}) == NormalNim::Move{11, 7 ^ 3 ^ 2}), isTrue());
    CHECK((computer.winningMove({4, 8, 9}) == NormalNim::Move{4, 8 ^ 9}), isTrue());
    CHECK((computer.winningMove({4, 5, 8, 9}) == std::nullopt), isTrue());
}
