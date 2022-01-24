#pragma once

#include "Function.hpp"
#include "MyHashUtil.hpp" // std::hash<std::tuple<...>>

/* Memoized usage:

int f(int x) {
    // some expensive computation where you want to cache the result into <something>
    return x + 1;
}

int g(std::vector<int> const& in) {
    int total = 0;
    Memoized<f> f_mem; // caches results from f
    for (int x : in) {
        total += f_mem(x); // operator() to use the Memoized object
    }
    return total;
}

*/
template<auto f>
struct Memoized {
  public:
    using In = Std_Tuple<f>;
    using Out = Output<f>;

  public:
    template<class... Ins, class = std::enable_if_t<std::is_constructible_v<In, Ins...>>>
    Out operator()(Ins&&... ins) {
	if (auto it = fEntries.find({ins...}); it != fEntries.end()) {
	    return it->second;
	}
	auto out = f(ins...);
	fEntries.insert({{ins...}, out});
	return out;
    }
    
  private:
    std::unordered_map<In, Out> fEntries;
};

/* RecursiveMemoized usage - note required name "rawFcn", recursive calls ("recFcn" or "operator()"), and inheritance

class X : public RecursiveMemoized<X, long, int> {
  public:
    long rawFcn(int x) {
        // some recursive call that uses the same inputs repeatedly
        if (x < 2) {
            return x;
        } else {
	    // recursive memoized call via operator() or call to recFcn()
            return (*this)(x - 1) + recFcn(x - 2);
        }
    }
};

int g(std::vector<int> const& in) {
    int total = 0;
    X x; // caches results from X::rawFcn
    for (int a : in) {
        total += x(a); // operator() to use the RecursiveMemoized object
    }
    return total;
}

*/
template<class X, class Out, class... In>
struct RecursiveMemoized {
  public:
    Out operator()(In const&... ins) {
        return fMemory(static_cast<X*>(this), ins...);
    }

  protected:
    Out recFcn(In const&... ins) {
	return (*this)(ins...);
    }

  private:
    static Out recFcnInternal(X* memory, In const&... ins) {
	return memory->rawFcn(ins...);
    }

  private:
    Memoized<recFcnInternal> fMemory;
};

