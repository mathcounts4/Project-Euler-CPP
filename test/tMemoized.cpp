#include "../Memoized.hpp"
#include "harness.hpp"

#include <any>

static int expensiveComputationCounter = 0;
struct ResetExpensiveCounter {
  public:
    ~ResetExpensiveCounter() {
	expensiveComputationCounter = 0;
    }
};

static int expensiveAdd(int x, int y) {
    ++expensiveComputationCounter;
    return x + y;
}

TEST(Memoized, Reuse) {
    ResetExpensiveCounter reset;
    Memoized<expensiveAdd> expensiveAddMemoized;
    
    CHECK(expensiveAddMemoized(2, 3), equals(5));
    CHECK(expensiveComputationCounter, equals(1));
    
    CHECK(expensiveAddMemoized(2, 3), equals(5));
    CHECK(expensiveComputationCounter, equals(1));

    
    CHECK(expensiveAddMemoized(4, 5), equals(9));
    CHECK(expensiveComputationCounter, equals(2));
    
    CHECK(expensiveAddMemoized(2, 3), equals(5));
    CHECK(expensiveComputationCounter, equals(2));
}

static long expensiveRecursiveFib(int);
Memoized<expensiveRecursiveFib> expensiveRecursiveFibMemoized;
static long expensiveRecursiveFib(int x) {
    ++expensiveComputationCounter;
    if (x < 2) {
	return x;
    }
    return expensiveRecursiveFibMemoized(x - 1) + expensiveRecursiveFibMemoized(x - 2);
}

TEST(Memoized, Recursive) {
    ResetExpensiveCounter reset;
    
    CHECK(expensiveRecursiveFibMemoized(12), equals(144));
    CHECK(expensiveComputationCounter, equals(13));
    
    CHECK(expensiveRecursiveFibMemoized(69), equals(117669030460994L));
    CHECK(expensiveComputationCounter, equals(70));
}

TEST(Memoized, RecursiveDeclOnce) {
    class RecFib : public RecursiveMemoized<RecFib, long, int> {
      public:
	long rawFcn(int x) {
	    ++expensiveComputationCounter;
	    if (x < 2) {
		return x;
	    } else {
		return (*this)(x - 1) + recFcn(x - 2);
	    }
	}
    };
    
    ResetExpensiveCounter reset;
    RecFib fib;

    CHECK(fib(12), equals(144));
    CHECK(expensiveComputationCounter, equals(13));
    
    CHECK(fib(69), equals(117669030460994L));
    CHECK(expensiveComputationCounter, equals(70));
}
