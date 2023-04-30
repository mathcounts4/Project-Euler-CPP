#include "BulgarianSolitaire.hpp"

BulgarianSolitaire::Summary::Summary(UL minIndex, Vec<UI> fullSequence)
    : fMinIndex(minIndex)
    , fInitialSequence(fullSequence) {
}

BulgarianSolitaire::Summary::Summary(UL minIndex, Vec<UI> initSequence, UI baseValueForRepeatingSequence, std::list<LenAndNumPiles> const& piles, UI pilesIndex)
    : fMinIndex(std::min(minIndex, static_cast<UL>(pilesIndex)))
    , fInitialSequence(initSequence)
    , fBaseValueForRepeatingSequence(baseValueForRepeatingSequence)
    , fRepeatingSequence(baseValueForRepeatingSequence + 1) {
    UL idx = baseValueForRepeatingSequence + 1;
    UI valueToBeat = fBaseValueForRepeatingSequence;
    auto processPileSize = [&](UI pileSize) {
	B isIncremented = pileSize > valueToBeat;
	--valueToBeat;
	fRepeatingSequence[--idx] = isIncremented;
    };
    for (auto const& [lengthPlusIndex, numPiles] : piles) {
	for (UI i = 0; i < numPiles; ++i) {
	    processPileSize(lengthPlusIndex - pilesIndex + 1);
	}
    }
}

UI BulgarianSolitaire::Summary::at(UL m) const {
    m -= fMinIndex;
    if (m < fInitialSequence.size()) {
	return fInitialSequence[m];
    } else {
	m -= fInitialSequence.size();
	return fBaseValueForRepeatingSequence + fRepeatingSequence[m % fRepeatingSequence.size()];
    }
}

void BulgarianSolitaire::addElementWithCount(UI count) {
    // skip 0's
    if (count) {
	++fCountToNumElements[count];
    }
}

BulgarianSolitaire::Summary BulgarianSolitaire::summarize(UL min, UL max) const {
    std::list<LenAndNumPiles> piles;
    UI totalNumPiles = 0;
    UI totalNumBeans = 0;
    UI metric = 0;
    UI baseDiagonalHeight = 0;
    // reverse order so we get sz high-to-low
    for (auto const& [sz, numPiles] : fCountToNumElements) {
	piles.push_back(LenAndNumPiles{sz, numPiles});
	totalNumPiles += numPiles;
	totalNumBeans += sz * numPiles;
	for (UI i = 1; i <= numPiles; ++i) {
	    metric += sz * (sz + 1) / 2 + sz * baseDiagonalHeight;
	    ++baseDiagonalHeight;
	}
    }
    UI minMetric = 0;
    UI xValue = 1;
    {
	// 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, ...
	UI numXs = 0;
	for (UI i = 1; i <= totalNumBeans; ++i) {
	    minMetric += xValue;
	    if (++numXs == xValue) {
		++xValue;
		numXs = 0;
	    }
	}
    }

    Vec<UI> seqBeforeStable;
    for (UL index = 1; index <= max; ++index) {
	// check for lowest metric = stable
	if (metric == minMetric) {
	    return {min, seqBeforeStable, xValue - 1, piles, static_cast<UI>(index)};
	}
	auto numToInsert = totalNumPiles;
	auto lengthPlusIndexToInsert = numToInsert + static_cast<UI>(index);
	// insert new pile
	++totalNumPiles;
	for (auto it = piles.begin(), end = piles.end(); ; ++it) {
	    if (it == end) {
		piles.insert(it, {lengthPlusIndexToInsert, 1u});
		break;
	    }
	    auto pileLengthPlusIndex = it->fLengthPlusIndex;
	    if (pileLengthPlusIndex < lengthPlusIndexToInsert) {
		piles.insert(it, {lengthPlusIndexToInsert, 1u});
		break;
	    } else if (pileLengthPlusIndex == lengthPlusIndexToInsert) {
		++it->fNumPiles;
		break;
	    } else {
		metric -= (pileLengthPlusIndex - lengthPlusIndexToInsert) * it->fNumPiles;
	    }
	}
	// remove empty piles
	if (piles.back().fLengthPlusIndex == index) {
	    totalNumPiles -= piles.back().fNumPiles;
	    piles.pop_back();
	}
	if (index >= min) {
	    seqBeforeStable.push_back(numToInsert);
	}
    }
    return {min, seqBeforeStable};
}

std::ostream& Class<BulgarianSolitaire>::print(std::ostream& os, BulgarianSolitaire const& t) {
    Proxy proxy;
    for (auto const& [sz, numPiles] : t.getCountToNumElements()) {
	for (UI i = 0; i < numPiles; ++i) {
	    proxy.insert(sz);
	}
    }
    return os << proxy;
}

Optional<BulgarianSolitaire> Class<BulgarianSolitaire>::parse(std::istream& is) {
    auto proxy = Class<Proxy>::parse(is);
    if (!proxy) {
	return Failure(proxy.cause());
    }
    BulgarianSolitaire bs;
    for (auto x : *proxy) {
	bs.addElementWithCount(x);
    }
    return bs;
}

std::string Class<BulgarianSolitaire>::name() {
    return "BulgarianSolitaire";
}

std::string Class<BulgarianSolitaire>::format() {
    return Class<Proxy>::format();
}

std::ostream& Class<BulgarianSolitaire::Summary>::print(std::ostream& os, BulgarianSolitaire::Summary const& t) {
    if (t.fMinIndex > 1) {
	os << "starting from " << t.fMinIndex << ": ";
    }
    os << "{";
    auto delim = "";
    for (auto x : t.fInitialSequence) {
	os << delim << x;
	delim = ",";
    }
    if (!t.fRepeatingSequence.empty()) {
	os << delim << "[";
	delim = "";
	for (auto x : t.fRepeatingSequence) {
	    os << delim << t.fBaseValueForRepeatingSequence + x;
	    delim = ",";
	}
	os << "]*";
    }
    os << "}";
    return os;
}

Optional<BulgarianSolitaire::Summary> Class<BulgarianSolitaire::Summary>::parse(std::istream&) {
    return Failure("Unimplemented");
}

std::string Class<BulgarianSolitaire::Summary>::name() {
    return "BulgarianSolitaire::Summary";
}

std::string Class<BulgarianSolitaire::Summary>::format() {
    return "<unimplemented>";
}
