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
#include <stdexcept>

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
                char const* data = Move{i, j} == move ? "X" /* or unicode not-filled-in square */ : " ";
                char const* backgroundColor = "0"; // black
                char textColor = '1'; // red
                switch (fTiles[i][j]) {
                  case 'B':
                    backgroundColor = "0"; // black
                    break;
                  case 'b':
                    backgroundColor = "4"; // blue
                    break;
                  case 'G':
                    backgroundColor = "2"; // green
                    break;
                  case 'g':
                    // https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
                    backgroundColor = "8;5;248"; // gray
                    break;
                  case 'O':
                    // https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
                    backgroundColor = "8;5;203"; // orange
                    break;
                  case 'P':
                    backgroundColor = "5"; // magenta
                    break;
                  case 'R':
                    backgroundColor = "1"; // red
                    textColor = '4'; // blue
                    break;
                  case 'W':
                    backgroundColor = "7"; // white
                    break;
                  case 'Y':
                    // https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
                    backgroundColor = "8;5;11"; // yellow
                    break;
                  default: {
                      auto failureString = std::string("Unknown color ") + fTiles[i][j];
                      std::cerr << failureString << std::endl;
                      throw std::logic_error(failureString);
                  }
                }
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
                      // Black cycles right (wrapping around)
                      auto copy = *this;
                      std::swap(copy.fTiles[i][2], copy.fTiles[i][1]);
                      std::swap(copy.fTiles[i][1], copy.fTiles[i][0]);
                      processNext(copy, move);
                      break;
                  }
                  case 'b': {
                      // Blue: copy color of center
                      auto copy = *this;
                      copy.fTiles[i][j] = copy.fTiles[1][1];
                      processNext(copy, move);
                      break;
                  }
                  case 'G': {
                      // green swaps with opposite tile
                      auto copy = *this;
                      std::swap(copy.fTiles[i][j], copy.fTiles[2-i][2-j]);
                      processNext(copy, move);
                      break;
                  }
                  case 'g': {
                      // gray: does nothing
                      break;
                  }
                  case 'O': {
                      // orange becomes most common direct neighbor (not including diagonal), if exclusive
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
                  case 'P': {
                      // purple moves down (stops at bottom)
                      if (i < 2) {
                          auto copy = *this;
                          std::swap(copy.fTiles[i][j], copy.fTiles[i+1][j]);
                          processNext(copy, move);
                      }
                      break;
                  }
                  case 'R': {
                      // red: transforms black -> red, and white -> black
                      auto copy = *this;
                      for (std::size_t x = 0; x < 3; ++x) {
                          for (std::size_t y = 0; y < 3; ++y) {
                              auto& tile = copy.fTiles[x][y];
                              switch (tile) {
                                case 'B':
                                  tile = 'R';
                                  break;
                                case 'W':
                                  tile = 'B';
                                  break;
                              }
                          }
                      }
                      processNext(copy, move);
                      break;
                  }
                  case 'W': {
                      // white becomes gray, and swaps gray/white among direct neighbors (not including diagonal)
                      auto copy = *this;
                      auto swapGrayWhite = [&copy](std::size_t x, std::size_t y) {
                          char& c = copy.fTiles[x][y];
                          if (c == 'g') {
                              c = 'W';
                          } else if (c == 'W') {
                              c = 'g';
                          }
                      };
                      swapGrayWhite(i, j);
                      if (i > 0) {
                          swapGrayWhite(i-1, j);
                      }
                      if (i < 2) {
                          swapGrayWhite(i+1, j);
                      }
                      if (j > 0) {
                          swapGrayWhite(i, j-1);
                      }
                      if (j < 2) {
                          swapGrayWhite(i, j+1);
                      }
                      processNext(copy, move);
                      break;
                  }
                  case 'Y': {
                      // yellow moves up (stops at top)
                      if (i > 0) {
                          auto copy = *this;
                          std::swap(copy.fTiles[i][j], copy.fTiles[i-1][j]);
                          processNext(copy, move);
                      }
                      break;
                  }
                  default: {
                    auto failureString = std::string("Unknown color ") + fTiles[i][j];
                    std::cerr << failureString << std::endl;
                    throw std::logic_error(failureString);
                  }
                }
            }
        }
    }
};

struct Puzzle {
    State fStart;
    char fWinningColor;
};

static void solve(Puzzle const& puzzle) {
    std::map<State, std::optional<std::pair<State, Move>>> visited{{puzzle.fStart, std::nullopt}};
    std::set<State> states{puzzle.fStart};
    while (!states.empty()) {
        std::set<State> newStates;
        for (auto const& state : states) {
            if (state.wins([&puzzle](char c) { return c == puzzle.fWinningColor; })) {
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
                std::cout << "Found solution: " << moves.size() << " moves:" << std::endl;
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
    std::cout << "Could not find a solution for:" << std::endl;
    puzzle.fStart.printWithMoveShown({3, 3});
}

static Puzzle innerSanctumRoom2() {
    return {
        {
            {
                {
                    { 'g', 'G', 'g' },
                    { 'O', 'R', 'O' },
                    { 'W', 'G', 'B' }
                }
            }
        }, 'R'};
}

static Puzzle innerSanctumRoom8() {
    return {
        {
            {
                {
                    { 'G', 'W', 'G' },
                    { 'W', 'O', 'O' },
                    { 'W', 'B', 'P' }
                }
            }
        },
        'G'};
}

static Puzzle hardOneOnline() {
    // https://www.reddit.com/r/BluePrince/comments/1kefpbv/i_made_a_puzzle_box_solver/
    return {
        {
            {
                {
                    { 'O', 'g', 'b' },
                    { 'b', 'O', 'B' },
                    { 'Y', 'g', 'G' }
                }
            }
        },
        'O'};
}

int main() {
    solve(innerSanctumRoom2());
    solve(innerSanctumRoom8());
    solve(hardOneOnline());
    return 0;
}
