#include "U_Map.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

#include <cmath>

/*
 * A segment tree is a way to represent a set of ordered nodes and some associated data with some efficient operations:
 * 1. Apply a commutative, associative operation to a range
 * 2. Query the operation result on the entire range
 * 3. Query the operation result on an individual element
 *
 * Wikipedia describes it as good at storing a list of possibly-overlapping segments, and efficiently calculating which segments contain a particular point.
 * This is a specific application of the above, where the operation is "membership in a particular range" and we query the result on a particular element.
 */

// Stores minimum N = 2^k leaves where n ≤ N (n inputs)
template<class Key, class Data>
struct SegmentTree {
  public:
    template<class Keys = std::initializer_list<Key>>
    SegmentTree(Keys&& sortedKeys) {
	UI n = 0;
	for (auto&& key : sortedKeys) {
	    fKeyToIndex.emplace(key, n++);
	    fKeys.push_back(key);
	}
	if (n < 2) {
	    n = 2;
	}
	fN = 2U << static_cast<UI>(std::log2(n - 1));
	fData.resize(2 * fN);
	fRanges.resize(2 * fN);
	for (UI i = fN; i < 2 * fN; ++i) {
	    fRanges[i] = {i - fN, i - fN};
	}
	for (UI i = fN; --i; ) {
	    fRanges[i] = {fRanges[2*i].first, fRanges[2*i+1].second};
	}
    }
    virtual ~SegmentTree() = default;

    // modifyRaw is called on a node whose complete range is covered by the key range (but is not called on its children)
    // This method branches along the tree to hit the whole key range across different tree nodes
    // O(log(N)) nodes are processed
    void modify(Key const& startKey, Key const& endKey) {
	auto startIndex = fKeyToIndex.at(startKey);
	auto endIndex = fKeyToIndex.at(endKey);
	if (startIndex > endIndex) {
	    std::swap(startIndex, endIndex);
	}
        modifyIndices(rootIndex(), startIndex, endIndex);
    }

    // aggregator is called on the data of all ranges that contain the desired key
    // O(log(N)) nodes are processed
    template<class Aggregator>
    void query(Key const& key, Aggregator&& f) const {
	auto index = fKeyToIndex.at(key);
	queryIndex(rootIndex(), index, f);
    }

    Data const& queryTop() const {
	return data(rootIndex());
    }

  protected:
    Key const& startKey(UI index) const {
	return fKeys[keyIndexRange(index).first];
    }
    Key const& endKey(UI index) const {
	return fKeys[keyIndexRange(index).second];
    }

    UI child1(UI index) const {
	return index * 2;
    }
    UI child2(UI index) const {
	return index * 2 + 1;
    }

    Data& data(UI index) {
	return fData[index];
    }
    Data const& data(UI index) const {
	return fData[index];
    }

    virtual void modifyRaw(UI index) = 0;
    virtual void computeDerivedForLeaf(UI /* index */) {}
    virtual void computeDerivedFromChildrenAndRaw(UI /* index */) {}

  private:
    void modifyIndices(UI current, UI keyIndexStart, UI keyIndexEnd) {
	auto [curKeyIdxMin, curKeyIdxMax] = keyIndexRange(current);
	if (keyIndexStart <= curKeyIdxMin && curKeyIdxMax <= keyIndexEnd) {
	    // Apply to this whole subtree
	    modifyRaw(current);
	    if (isLeaf(current)) {
		computeDerivedForLeaf(current);
	    } else {
		// not a leaf - apply the joiner
		computeDerivedFromChildrenAndRaw(current);
	    }
	} else if (keyIndexEnd < curKeyIdxMin || curKeyIdxMax < keyIndexStart) {
	    return;
	} else {
	    modifyIndices(child1(current), keyIndexStart, keyIndexEnd);
	    modifyIndices(child2(current), keyIndexStart, keyIndexEnd);
	    // Current derived data needs to be modified as a result of children data being modified
	    computeDerivedFromChildrenAndRaw(current);
	}
    }

    template<class Aggregator>
    void queryIndex(UI current, UI target, Aggregator&& f) const {
	auto [curMin, curMax] = keyIndexRange(current);
	if (curMin <= target && target <= curMax) {
	    f(fData[current]);
	    if (!isLeaf(current)) {
		queryIndex(child1(current), target, f);
		queryIndex(child2(current), target, f);
	    }
	}
    }

  private:
    UI rootIndex() const {
	return 1;
    }
    
    B isLeaf(UI index) const {
	return index >= fN;
    }
    
    auto keyIndexRange(UI index) const {
	return fRanges.at(index);;
    }

  private:
    // maps keys to [0, n)
    U_Map<Key, UI> fKeyToIndex;
    // maps [0, n) to keys
    Vec<Key> fKeys;

    // 
    UI fN;
    
    // N = 4, fData has size 8:
    //       1
    //    2     3
    //  4  5  6  7
    // Leaves (keys) have indices
    //  0  1  2  3
    Vec<Data> fData;

    // Maps each i in [1, 2N) to the range of leaves
    // 1 -> {0,3}
    // 2 -> {0,1}
    // 3 -> {2,3}
    // 4 -> {0,0}
    // 5 -> {1,1}
    // 6 -> {2,2}
    // 7 -> {3,3}
    Vec<std::pair<UI, UI>> fRanges;
};
