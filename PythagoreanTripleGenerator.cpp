#include "PythagoreanTripleGenerator.hpp"

#include "Matrix/Matrix.hpp"

Vec<PythagoreanTriple> primitivePythagoreanTriplesUpToPerimeter(UI maxPerimeter) {
    // https://en.wikipedia.org/wiki/Tree_of_primitive_Pythagorean_triples
    Vec<PythagoreanTriple> result;
    using Triple = Matrix<SL,3,1>;
    using Transform = Matrix<SL,3,3>;
    // We recorder some rows of the matrices in order to always produces triples that have a<b<c.
    Transform A{{1, -2, 2},
		{2, -1, 2},
		{2, -2, 3}};
    Transform B{{2, 1, 2},
		{1, 2, 2},
		{2, 2, 3}};
    Transform C{{-2, 1, 2},
		{-1, 2, 2},
		{-2, 2, 3}};
    Vec<Triple> worklist({{{3}, {4}, {5}}});
    while (!worklist.empty()) {
	auto triple = worklist.back();
	worklist.pop_back();
	auto a = triple[0][0];
	auto b = triple[1][0];
	auto c = triple[2][0];
	if (a + b + c <= static_cast<SL>(maxPerimeter)) {
	    result.push_back({static_cast<UI>(a), static_cast<UI>(b), static_cast<UI>(c)});
	    worklist.push_back(A * triple);
	    worklist.push_back(B * triple);
	    worklist.push_back(C * triple);
	}
    }
    return result;
}

