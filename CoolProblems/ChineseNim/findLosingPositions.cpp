#include <iostream>
#include <set>
#include <vector>

struct TempReplace {
    TempReplace(std::multiset<int>& m, int x, int y)
    : fSet(m)
    , fX(x)
    , fY(y) {
        fSet.erase(fSet.find(fX));
        if (fY > 0) {
            fSet.insert(fY);
        }
    }
    ~TempReplace() {
        if (fY > 0) {
            fSet.erase(fSet.find(fY));
        }
        fSet.insert(fX);
    }

    std::multiset<int>& fSet;
    int fX;
    int fY;
};

int main() {
    int nnn = 100;
    std::set<std::multiset<int>> losers; 
    for (int numPiles = 0; numPiles <= 2; ++numPiles) {
        std::vector<int> m(numPiles, 1);
        while (true) {
            if (![&]() {
                std::multiset<int> ms(m.begin(), m.end());
                for (int p1 = 0; p1 < numPiles; ++p1) {
                    for (int toRemove = 1, orig = m[p1]; toRemove <= orig; ++toRemove) {
                        TempReplace r1(ms, orig, orig - toRemove);
                        if (losers.count(ms)) {
                            return true;
                        }
                    }
                    for (int p2 = p1 + 1; p2 < numPiles; ++p2) {
                        for (int toRemove = 1, orig1 = m[p1], orig2 = m[p2]; toRemove <= m[p1]; ++toRemove) {
                            TempReplace r1(ms, orig1, orig1 - toRemove);
                            TempReplace r2(ms, orig2, orig2 - toRemove);
                            if (losers.count(ms)) {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }()) {
                std::multiset<int> ms(m.begin(), m.end());
                losers.insert(ms);
                std::cout << "{";
                char const* prefix = "";
                for (int x : ms) {
                    std::cout << prefix << x;
                    prefix = ", ";
                }
                std::cout << "}" << std::endl;
            }
            bool modified = false;
            for (int i = numPiles; i--; ) {
                if (m[i] < nnn) {
                    int newVal = m[i] + 1;
                    for (int j = i; j < numPiles; ++j) {
                        m[j] = newVal;
                    }
                    modified = true;
                    break;
                }
            }
            if (!modified) {
                break;
            }
        }
    }
}

