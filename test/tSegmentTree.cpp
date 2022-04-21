#include "../SegmentTree.hpp"
#include "harness.hpp"

TEST(SegmentTree, Add) {
    struct Tree : public SegmentTree<SI, SL> {
	using SegmentTree::SegmentTree;
	void modifyRaw(UI index) override {
	    ++data(index);
	}
    };
    Tree tree({0, 1, 2, 3, 4, 5});
    tree.modify(1, 5);
    tree.modify(1, 3);
    tree.modify(3, 4);
    tree.modify(1, 1);

    auto query = [&tree](SI key) {
	SL total = 0;
	tree.query(key, [&total](SL x) { total += x; });
	return total;
    };
    CHECK(query(0), equals(0));
    CHECK(query(1), equals(3));
    CHECK(query(2), equals(2));
    CHECK(query(3), equals(3));
    CHECK(query(4), equals(2));
    CHECK(query(5), equals(1));
}

TEST(SegmentTree, BigAdd) {
    D nWithSlowSquare = 3e5;

    // Perform O(n) insertions and queries - should be fast
    //   since each should take O(log(n))
    // If either took O(n), this function would take O(n^2) time to run, and be slow.
    
    struct Tree : public SegmentTree<D, SL> {
	using SegmentTree::SegmentTree;
	void modifyRaw(UI index) override {
	    ++data(index);
	}
    };
    
    Vec<D> keys;
    for (D i = 0; i < nWithSlowSquare; i += 0.5) {
	keys.push_back(i);
    }
    
    Tree tree(keys);
    
    for (D i = 0; i < nWithSlowSquare; i += 0.5) {
	// also checks that .modify() works if the first key > second key
	tree.modify(i, nWithSlowSquare - 0.5 - i);
    }

    auto query = [&tree](D key) {
	SL total = 0;
	tree.query(key, [&total](SL x) { total += x; });
	return total;
    };
    for (D i = 0; i < nWithSlowSquare; i += 0.5) {
	CHECK(query(i), equals(std::min(i, nWithSlowSquare - 0.5 - i) * 4 + 2));
    }
}

TEST(SegmentTree, PropagateUp) {
    // Add over a range at a time
    // But also keep a record of the total sum by modifying up the tree as we insert

    struct Data {
	SL fRaw;
	SL fDerivedTotal;
    };
    struct Tree : public SegmentTree<SI, Data> {
	using SegmentTree::SegmentTree;
	void modifyRaw(UI index) override {
	    ++data(index).fRaw;
	}
	void computeDerivedForLeaf(UI index) override {
	    auto& element = data(index);
	    element.fDerivedTotal = element.fRaw;
	}
	void computeDerivedFromChildrenAndRaw(UI index) override {
	    auto start = startKey(index);
	    auto end = endKey(index);
	    auto width = end - start + 1;
	    auto contribution = data(index).fRaw * width;
	    data(index).fDerivedTotal = data(child1(index)).fDerivedTotal + data(child2(index)).fDerivedTotal + contribution;
	}
    };
    Tree tree({0, 1, 2, 3, 4, 5});
    auto queryBigTotal = [&tree] () { return tree.queryTop().fDerivedTotal; };
    tree.modify(1, 5);
    CHECK(queryBigTotal(), equals(5));
    tree.modify(1, 3);
    CHECK(queryBigTotal(), equals(8));
    tree.modify(3, 4);
    CHECK(queryBigTotal(), equals(10));
    tree.modify(1, 1);
    CHECK(queryBigTotal(), equals(11));

    auto query = [&tree](SI key) {
	SL total = 0;
	tree.query(key, [&total](Data const& x) { total += x.fRaw; });
	return total;
    };
    CHECK(query(0), equals(0));
    CHECK(query(1), equals(3));
    CHECK(query(2), equals(2));
    CHECK(query(3), equals(3));
    CHECK(query(4), equals(2));
    CHECK(query(5), equals(1));
}
