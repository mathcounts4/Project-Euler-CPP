/* Problem:

With an n-note octave (repeating notes), how many distinct chords are there?
Chords are equivalent up to translation and rotation.

*/

#include <iostream>

int main() {
    int t = 0;
    int const n = 12;
    int count_per_notes[n+1]{};
    int const two_to_n = 1 << n;
    for (int i = 0; i < two_to_n; ++i) {
        for (int j = i, rot = 0; rot < n; ++rot) {
            j *= 2;
            auto top = j & two_to_n;
            j |= top >> n;
            j &= ~two_to_n;
            if (j < i) {
                // an inversion or translation of this chord is already counted
                goto Failure;
            }
        }
        ++t;
        ++count_per_notes[__builtin_popcount(i)];
        Failure:;
    }
    std::cout << t << std::endl;
    for (int i = 0; i <= n; ++i) {
        std::cout << i << ": " << count_per_notes[i] << std::endl;
    }
    // Now we see that, of the c(12,x) options for x notes,
    //   each chord appears repeated in the list 1 to 12 times.
    // Specifically, if x is repeating within itself,
    //   (like a fully diminished seventh, with x=4)
    //   it only appears [the length of its cycle] times.
    // So #chords(n possible notes) =
    //   sum_{C over all chords} 1/cycle_length(C)
    // If gcd(n, x) = 1, cycle_length = n,
    //   so there are c(n, x) / n chords with x notes.
    // But how many f(L) exist with each cycle length L and any number of notes?
    // We see that:
    //   sum_{d|L} f(d) = sum_{C over all chords} 1 = 2^L
    // So by Mobius inversion,
    //   f(L) = sum_{d|L} mu(d) 2^(L/d)
    // We care about:
    //   sum_{C over all chords} 1/cycle_length(C)
    //   = sum_{d|n} f(d)/d
    //   = sum_{d|n} (1/d) sum_{e|d} mu(e) 2^(d/e)
    //   = sum_{e|n} mu(e) sum_{d, e|d|n} 2^(d/e)/d
    //   = sum_{e|n} (mu(e)/e) sum_{k|n/e} 2^k/k
    // Check: n=12
    //   e = 1, 2, 3, 6 (e = 4 and 12 have mu(e)=0)
    //   1/1 * (2^1/1 + 2^2/2 + 2^3/3 + 2^4/4 + 2^6/6 + 2^12/12)
    //    - 1/2 * (2^1/1 + 2^2/2 + 2^3/3 + 2^6/6)
    //    - 1/3 * (2^1/1 + 2^2/2 + 2^4/4)
    //    + 1/6 * (2^1/1 + 2^2/2)
    //   = 352
    // Also see Burnside_Lemma.pdf
}
