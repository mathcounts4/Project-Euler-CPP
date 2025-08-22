// Black - pushes right (rotates row) (cycles)
// Purple - swaps with one below (stops at bottom)
// Green - swaps with opposite
// White - does nothing
// Yellow - becomes the most common adjacent color (veritcal or horizontal)

// Game controls:
// 4 corner buttons:
// * Light up with color if correct color is in that corner (goes out if this piece is shifted elsewhere)
// * Reset to starting state if correct color is not in that corner

#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <set>

struct Move {
    std::size_t fI;
    std::size_t fJ;

    friend auto operator<=>(Move const&, Move const&) = default;
};

struct State {
    std::array<std::array<char, 3>, 3> fTiles;

    friend auto operator<=>(State const&, State const&) = default;

    bool wins(std::function<bool(char)> checkColor) const {
        auto color = fTiles[0][0];
        if (!checkColor(color)) {
            return false;
        }
        return color == fTiles[0][2] && color == fTiles[2][0] && color == fTiles[2][2];
    }

    void printWithMoveShown(Move const& move) const {
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                char const* data = Move{i, j} == move ? "X" /* unicode not-filled-in square */ : " ";
                char backgroundColor = '0'; // black
                switch (fTiles[i][j]) {
                  case 'B':
                    backgroundColor = '0'; // black
                    break;
                  case 'P':
                    backgroundColor = '5'; // magenta
                    break;
                  case 'G':
                    backgroundColor = '2'; // green
                    break;
                  case 'W':
                    backgroundColor = '7'; // white
                    break;
                  case 'Y':
                    backgroundColor = '3'; // yellow
                    break;
                }
                char textColor = '1'; // red
		std::cout << "\x1b[3" << textColor << ";4" << backgroundColor << "m";
                std::cout << data;
		std::cout << "\x1b[0m"; // reset
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void visitNext(std::function<void(State const&, Move const&)> processNext) const {
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                Move move{i, j};
                
                switch (fTiles[i][j]) {
                  case 'B': {
                      auto copy = *this;
                      std::swap(copy.fTiles[i][2], copy.fTiles[i][1]);
                      std::swap(copy.fTiles[i][1], copy.fTiles[i][0]);
                      processNext(copy, move);
                      break;
                  }
                  case 'P': {
                      if (i < 2) {
                          auto copy = *this;
                          std::swap(copy.fTiles[i][j], copy.fTiles[i+1][j]);
                          processNext(copy, move);
                      }
                      break;
                  }
                  case 'G': {
                      auto copy = *this;
                      std::swap(copy.fTiles[i][j], copy.fTiles[2-i][2-j]);
                      processNext(copy, move);
                      break;
                  }
                  case 'W': {
                      break;
                  }
                  case 'Y': {
                      std::map<char, int> counts;
                      if (i > 0) {
                          ++counts[fTiles[i-1][j]];
                      }
                      if (i < 2) {
                          ++counts[fTiles[i+1][j]];
                      }
                      if (j > 0) {
                          ++counts[fTiles[i][j-1]];
                      }
                      if (j < 2) {
                          ++counts[fTiles[i][j+1]];
                      }
                      int maxCount = 0;
                      std::optional<char> mostCommonNeightbor;
                      for (auto const& [color, count] : counts) {
                          if (count > maxCount) {
                              mostCommonNeightbor = color;
                              maxCount = count;
                          } else if (count == maxCount) {
                              mostCommonNeightbor = std::nullopt;
                          }
                      }
                      if (mostCommonNeightbor) {
                          auto copy = *this;
                          copy.fTiles[i][j] = *mostCommonNeightbor;
                          processNext(copy, move);
                      }
                      break;
                  }
                }
            }
        }
    }
};

static void solveInnerSanctumRoom8() {
    State start = {
        {
            {
                { 'G', 'W', 'G' },
                { 'W', 'Y', 'Y' },
                { 'W', 'B', 'P' }
            }
        }
    };

    std::map<State, std::optional<std::pair<State, Move>>> visited{{start, std::nullopt}};
    std::set<State> states{start};
    while (!states.empty()) {
        std::set<State> newStates;
        for (auto const& state : states) {
            if (state.wins([](char c) { return c == 'G'; })) {
                std::vector<std::pair<State, Move>> moves;
                State const* s = &state;
                while (true) {
                    if (auto const& prevAndMove = visited.at(*s)) {
                        moves.push_back(*prevAndMove);
                        s = &prevAndMove->first;
                    } else {
                        break;
                    }
                }
                std::reverse(moves.begin(), moves.end());
                for (auto const& [st, mv] : moves) {
                    st.printWithMoveShown(mv);
                }
                state.printWithMoveShown({3, 3});
                return;
            }
            auto processNext = [&](State const& nextState, Move const& move) {
                bool isNew = visited.insert({nextState, std::make_optional<std::pair<State, Move>>({state, move})}).second;
                if (isNew) {
                    newStates.insert(nextState);
                }
            };
            state.visitNext(processNext);
        }
        std::swap(states, newStates);
    }
}

int main() {
    solveInnerSanctumRoom8();
    return 0;
}
