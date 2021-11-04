#include "Board.hpp"
#include "Keyboard.hpp"
#include "MouseController.hpp"
#include "View.hpp"
#include "../DebugPrint.hpp"
#include "../Now.hpp"
#include "../Rand.hpp"
#include "../Wait.hpp"

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

#define CAN_REVEAL_SURROUNDING false

Board::Board(unsigned int width, unsigned int height, unsigned int mines)
    : fWidth(width)
    , fHeight(height)
    , fTiles(height, std::vector<Data>(width, UNKNOWN))
    , fCanRevealSurrounding(CAN_REVEAL_SURROUNDING)
    , fMinesLeft(mines) {
}

Board::AdjInfo Board::adj(BoardPosition const& pos) const {
    unsigned int numUnknown = 0;
    unsigned int numMines = 0;
    for (auto const& n : neighbors(pos)) {
	switch (getData(n)) {
	  case UNKNOWN:
	    ++numUnknown;
	    break;
	  case MINE:
	    ++numMines;
	    break;
	}
    }
    return {numUnknown, numMines};
}

std::vector<Board::Move> Board::movesFromTryingPossibilities(BoardPosition const& pos, unsigned int numUnknownMines) const {
    // Run all possible scenarios for neighboring unknowns.
    // If all valid scenarios have the same value for a square,
    // that value is guaranteed.
		
    // Covers cases like:
		
    // ????
    // x12y -> top-left ? is safe, top-right ? is bomb
    // abcd

    // a2b
    // ???
    // c3? -> ? on this line is bomb
    // def
    // ...

    struct MineInfo {
	unsigned int fMaxAdjUnknownMines;
	unsigned int fMaxAdjUnknownSafe;
    };

    std::vector<BoardPosition> unknownNeighbors;
    std::unordered_map<BoardPosition, MineInfo> affected;
    for (auto const& neighbor : neighbors(pos)) {
	if (isUnknown(neighbor)) {
	    unknownNeighbors.push_back(neighbor);
	    for (auto const& nn : neighbors(neighbor)) {
		auto const nnVal = getData(nn);
		if ('0' <= nnVal && nnVal <= '8') {
		    if (!affected.count(nn)) {
			unsigned int const expectedMines = static_cast<unsigned int>(nnVal - '0');
			auto [numUnknown, numMines] = adj(nn);
			auto numAdjMines = expectedMines - numMines;
			auto numAdjSafe = numUnknown - numAdjMines;
			affected.insert({nn, {numAdjMines, numAdjSafe}});
		    }
		}
	    }
	}
    }

    auto const numUnknownNeighbors = unknownNeighbors.size();
    unsigned int possibilities = 1U << numUnknownNeighbors;
    std::vector<unsigned char> validPossibilities;
    for (unsigned int i = 0; i < possibilities; ++i) {
        if (static_cast<unsigned int>(__builtin_popcount(i)) != numUnknownMines) {
	    continue;
	}
	bool isValid = true;
	auto affectedCopy = affected;
	for (unsigned int ni = 0; ni < numUnknownNeighbors; ++ni) {
	    auto const neighbor = unknownNeighbors[ni];
	    auto const mask = 1U << ni;
	    bool const isMine = (i & mask) != 0U;
	    for (auto const& nn : neighbors(neighbor)) {
		if (auto it = affectedCopy.find(nn); it != affectedCopy.end()) {
		    MineInfo& info = it->second;
		    unsigned int& maxCount = isMine ? info.fMaxAdjUnknownMines : info.fMaxAdjUnknownSafe;
		    if (maxCount > 0) {
			--maxCount;
		    } else {
			isValid = false;
		    }
		}
	    }
	}
	if (isValid) {
	    validPossibilities.push_back(static_cast<UC>(i));
	}
    }

    auto const allBitsSet = static_cast<unsigned char>(possibilities - 1);
    auto alwaysMine = allBitsSet;
    auto alwaysSafe = allBitsSet;
    for (auto x : validPossibilities) {
	alwaysMine &= x;
	alwaysSafe &= ~x;
    }

    std::vector<Move> moves;
    for (unsigned int n = 0; n < numUnknownNeighbors; ++n) {
	auto const mask = static_cast<unsigned char>(1) << n;
	if (alwaysSafe & mask) {
	    moves.emplace_back(unknownNeighbors[n], Mark::REVEAL);
	} else if (alwaysMine & mask) {
	    moves.emplace_back(unknownNeighbors[n], Mark::MINE);
	}
    }
    return moves;
}

std::vector<Board::Move> Board::nextCorrectMoves() {
    std::unordered_set<BoardPosition> willBeRevealed;
    // As we add to this set, we also mark the board:
    // Mines are marked as mines,
    // Safe unknown spaces are marked as SAFE_UNKNOWN.
    // Spaces to reveal all around (after marking all mines in the list) are already marked with their number.

    bool anyChange = true;
    auto mark = [this, &willBeRevealed, &anyChange](BoardPosition const& position, Data data) {
	if (willBeRevealed.emplace(position).second) {
	    this->mark(position, data);
	    anyChange = true;
	}
    };
    while (anyChange) {
	anyChange = false;
	for (unsigned int y = 0; y < fHeight; ++y) {
	    for (unsigned int x = 0; x < fWidth; ++x) {
		auto val = getData({x, y});
		if (!('0' <= val && val <= '8')) {
		    continue;
		}
		BoardPosition const pos{x, y};
		// first: look for squares where all remaining neighbors are all mines or all safe
		auto expectedMines = static_cast<unsigned int>(val - '0');
		auto [numUnknown, numMines] = adj(pos);
		if (numUnknown == 0) {
		    continue;
		}
		if (expectedMines < numMines) {
		    throw_exception<std::logic_error>("(" + std::to_string(x) + "," + std::to_string(y) + "): found " + std::to_string(numMines) + " mines, but only expected " + std::to_string(expectedMines));
		}
		auto numUnknownMines = expectedMines - numMines;
		if (numUnknownMines == 0) {
		    if (fCanRevealSurrounding) {
			mark(pos, val);
		    }
		    for (auto const& n : neighbors(pos)) {
			if (isUnknown(n)) {
			    mark(n, SAFE_UNKNOWN);
			}
		    }
		} else if (numUnknownMines == numUnknown) {
		    for (auto const& n : neighbors(pos)) {
			if (isUnknown(n)) {
			    mark(n, MINE);
			}
		    }
		} else {
		    for (auto const& move : movesFromTryingPossibilities(pos, numUnknownMines)) {
			auto [markPos, markKind] = move;
			if (markKind == Mark::MINE) {
			    mark(markPos, MINE);
			} else if (markKind == Mark::REVEAL) {
			    mark(markPos, SAFE_UNKNOWN);
			}
		    }
		}
	    }
	}
    }
    
    std::vector<Move> moves;
    for (auto const& pos : willBeRevealed) {
	if (isMine(pos)) {
	    moves.emplace_back(pos, Mark::MINE);
	} else if (isSafeUnknown(pos)) {
	    bool needToReveal = true;
	    for (auto const& n : neighbors(pos)) {
		if (willBeRevealed.count(n) && !isMine(n) && !isSafeUnknown(n)) {
		    // revelaing all around neighbor n, so don't need to reveal this one by itself.
		    needToReveal = false;
		    break;
		}
	    }
	    if (needToReveal) {
		moves.emplace_back(pos, Mark::REVEAL);
	    }
	} else {
	    // should be a number, and we reveal surrounding squares
	    moves.emplace_back(pos, Mark::REVEAL_SURROUNDING);
	}
    }

    std::sort(moves.begin(), moves.end(), [](Move const& a, Move const& b) {
	auto const& [aPos, aMark] = a;
	auto const& [bPos, bMark] = b;
	auto preference = [](Mark m) {
	    switch (m) {
	      case Mark::MINE:
		return 1;
	      case Mark::REVEAL:
		return 2;
	      case Mark::REVEAL_SURROUNDING:
		return 3;
	    }
	};
	if (aMark != bMark) {
	    return preference(aMark) < preference(bMark);
	}
	return aPos < bPos;
    });
    
    return moves;
}

Optional<Board::Move> Board::nextGuessMove() const {
    using BombProbability = double;
    std::pair<BombProbability, std::vector<Move>> bestMoves{1, {}};
    auto considerMove = [&bestMoves](BombProbability bombProbability, Move const& move) {
	if (bombProbability < bestMoves.first) {
	    bestMoves = {bombProbability, {}};
	}
	if (bombProbability == bestMoves.first) {
	    bestMoves.second.push_back(move);
	}
    };
    auto bestMove = [&bestMoves]() -> Optional<Board::Move> {
	auto const& list = bestMoves.second;
	if (list.empty()) {
	    return Failure("no guess move");
	}
	return list[randPositiveLong(list.size()) - 1];
    };
    
    // reveal a random unknown square
    for (unsigned int y = 0; y < fHeight; ++y) {
	for (unsigned int x = 0; x < fWidth; ++x) {
	    auto data = getData({x, y});
	    if (data == UNKNOWN) {
		double maxProbFromNeighbors = -1;
		for (auto const& neighbor : neighbors({x, y})) {
		    auto neighborData = getData(neighbor);
		    if ('0' <= neighborData && neighborData <= '8') {
			unsigned int numMines = static_cast<unsigned int>(neighborData - '0');
			auto const& [numUnknown, foundMines] = adj(neighbor);
			auto minesLeft = numMines - foundMines;
			maxProbFromNeighbors = std::max(maxProbFromNeighbors, static_cast<double>(minesLeft) / numUnknown);
		    }
		}
		if (maxProbFromNeighbors < 0) {
		    // no known neighbors - treat just worse than 0.5
		    maxProbFromNeighbors = 0.51;
		}
		considerMove(maxProbFromNeighbors, {{x, y}, Mark::REVEAL});
	    }
	}
    }
    auto move = bestMove();
    if (move) {
	debug::cout << "No idea! Randomly revealing: " << move->first.x << " " << move->first.y << std::endl;
    }
    return move;
}

std::vector<BoardPosition> Board::neighbors(BoardPosition const& position) const {
    std::vector<BoardPosition> result;
    auto [x, y] = position;
    auto xBound = std::min(x + 2, fWidth);
    auto yBound = std::min(y + 2, fHeight);
    
    for (auto j = y - (y > 0); j < yBound; ++j) {
	for (auto i = x - (x > 0); i < xBound; ++i) {
	    if (i != x || j != y) {
		result.push_back({i, j});
	    }
	}
    }
    return result;
}

void Board::print() const {
    std::cout << "\n";
    std::cout << " " << std::string(width(), '-') << " " << "\n";
    auto const canColorFormat = isatty(fileno(stdout));
    for (auto const& row : fTiles) {
	std::cout << "|";
	for (Data data : row) {
	    if (!canColorFormat) {
		if (data == UNKNOWN) {
		    data = '?';
		} else if (data == '0') {
		    data = ' ';
		}
		std::cout << data;
	    } else {
		// canColorFormat
		char textColor = '0'; // black
		char const* extra = "";
		Optional<char const*> altData(Failure("No alt data"));
		switch (data) {
		  case '3':
		    textColor = '1'; // red
		    break;
		  case '2':
		    textColor = '2'; // green
		    break;
		  case '4':
		    textColor = '3'; // yellow
		    break;
		  case '1':
		    textColor = '4'; // blue
		    break;
		  case '5':
		    textColor = '5'; // magenta
		    break;
		  case '6':
		    textColor = '6'; // cyan
		    break;
		  case '7':
		  case '8':
		    textColor = '7'; // white
		    break;
		  case '0':
		    data = ' ';
		    break;
		  case UNKNOWN:
		    altData = "\u25A0"; // unicode square
		    textColor = '7'; // white
		    break;
		  case SAFE_UNKNOWN:
		    textColor = '7'; // white
		    break;
		  case MINE:
		    altData = "\u25A0"; // unicode square
		    textColor = '1'; // red
		    break;
		  case EXPLODED_MINE:
		    data = MINE;
		    textColor = '0'; // black
		    extra = ";41"; // background: red
		    break;
		}
		std::cout << "\x1b[3" << textColor << extra << "m";
		if (altData) {
		    std::cout << *altData;
		    //std::cout << "A";
		} else {
		    std::cout << data;
		}
		std::cout << "\x1b[0m"; // reset
	    }
	}
	std::cout << "|" << "\n";
    }
    std::cout << " " << std::string(width(), '-') << " " << "\n";
    std::cout << "\n";
    std::cout << std::flush;
}

bool Board::isDone() const {
    for (unsigned int y = 0; y < fHeight; ++y) {
	for (unsigned int x = 0; x < fWidth; ++x) {
	    if (isUnknown({x, y})) {
		return false;
	    }
	}
    }
    return true;
}

bool Board::isUnknown(BoardPosition const& position) const {
    return hasValue(position, UNKNOWN);
}

bool Board::isSafeUnknown(BoardPosition const& position) const {
    return hasValue(position, SAFE_UNKNOWN);
}

bool Board::isMine(BoardPosition const& position) const {
    return hasValue(position, MINE) || hasValue(position, EXPLODED_MINE);
}

void Board::mark(BoardPosition const& position, Data value) {
    if (0 <= value && value <= 8) {
	// normalize to '0'-'8'
	value += '0';
    }
    fTiles[position.y][position.x] = value;
    if (isMine(position)) {
	--fMinesLeft;
    }
}

Board::Data Board::getData(BoardPosition const& position) const {
    auto const& [x, y] = position;
    return fTiles[y][x];
}

bool Board::hasValue(BoardPosition const& position, Data value) const {
    return getData(position) == value;
}

std::optional<ScreenBoard> ScreenBoard::findFromScreen(Time maxTime, Time delayBetween) {
    auto start = now();
    while (timeFrom(start) < maxTime) {
	if (auto screenBoard = findFromCurrentScreen()) {
	    return screenBoard;
	}
	waitFor(delayBetween);
    }
    return std::nullopt;
}

ScreenBoard::MoveResult ScreenBoard::doNextMove() {
    auto nextMoves = fBoard.nextCorrectMoves();
    bool confident = !nextMoves.empty();
    if (nextMoves.empty()) {
	// if board isn't started, use a random square
	bool allUnknown = true;
	auto height = fBoard.height();
	auto width = fBoard.width();
	for (unsigned int y = 0; y < height; ++y) {
	    for (unsigned int x = 0; x < width; ++x) {
		if (!fBoard.isUnknown({x, y})) {
		    allUnknown = false;
		}
	    }
	}
	if (fGuessWhenUnsure || allUnknown) {
	    if (auto guess = fBoard.nextGuessMove()) {
		nextMoves.push_back(*guess);
	    }
	}
    }
    
    if (nextMoves.empty()) {
	return MoveResult::NONE_AVAILABLE;
    }

    // TODO: make set of to-be-revealed positions
    // prefer mines, then individual reveals not covered by another move, then group reveals.
    // When going super fast, making a mine then revealing adjacent sometimes fails to update fast enough.
    // If there are mines and group reveals but no individual reveals, add a dummy individual reveal.

    

    std::sort(nextMoves.begin(), nextMoves.end(), [](Board::Move const& x, Board::Move const& y) {
	auto [xPos, xKind] = x;
	auto [yPos, yKind] = y;
	auto xMine = xKind == Board::Mark::MINE;
	auto yMine = yKind == Board::Mark::MINE;
	auto xSurrounding = xKind == Board::Mark::REVEAL_SURROUNDING;
	auto ySurrounding = yKind == Board::Mark::REVEAL_SURROUNDING;
	if (xMine != yMine) {
	    // prefer mines first
	    return xMine;
	}
	if (xSurrounding != ySurrounding) {
	    // prefer surrounding over plain reveal
	    return xSurrounding;
	}
	auto [xx, xy] = xPos;
	auto [yx, yy] = yPos;
	if (xy != yy) {
	    return xy < yy;
	}
	return xx < yx;
    });
    
    std::vector<std::pair<BoardPosition, LookFor>> updateAfter;
    std::unordered_set<BoardPosition> revealedPositions;
    for (auto const& [boardPos, mark] : nextMoves) {
	switch (mark) {
	    // may not need to do this move, if the tile(s) are already revealed
	  case Board::Mark::REVEAL:
	  case Board::Mark::MINE: {
	      if (revealedPositions.count(boardPos)) {
		  continue;
	      }
	      break;
	  }
	  case Board::Mark::REVEAL_SURROUNDING: {
	      bool shouldReveal = false;
	      for (auto const& neighbor : fBoard.neighbors(boardPos)) {
		  if (!revealedPositions.count(neighbor)) {
		      shouldReveal = true;
		      break;
		  }
	      }
	      if (!shouldReveal) {
		  continue;
	      }
	      break;
	  }
	}
	if (!moveToBoardPosition(boardPos)) {
	    return MoveResult::MANUALLY_CANCELLED;
	}
	auto [x, y] = boardPos;
	switch (mark) {
	  case Board::Mark::REVEAL: {
	      debug::cout << "Revealing: " << x << " " << y << std::endl;
	      auto manualMoved = Mouse::cancellableLeftClick();
	      if (manualMoved == Mouse::Moved::YES) {
		  return MoveResult::MANUALLY_CANCELLED;
	      }
	      updateAfter.emplace_back(boardPos, LookFor::EXACT_SQUARE);
	      revealedPositions.insert(boardPos);
	      break;
	  }
	  case Board::Mark::MINE: {
	      debug::cout << "Marking as mine: " << x << " " << y << std::endl;
	      fBoard.mark(boardPos, Board::MINE);
	      auto manualMoved = Mouse::cancellableRightClick();
	      if (manualMoved == Mouse::Moved::YES) {
		  return MoveResult::MANUALLY_CANCELLED;
	      }
	      break;
	  }
	  case Board::Mark::REVEAL_SURROUNDING: {
	      debug::cout << "Revealing surrounding: " << x << " " << y << std::endl;
	      if (fUsesSpaceForSurrounding) {
		  // Pressing space too fast sometimes fails.
		  // Always wait, but speed up by a decent factor.
		  // But alwaysWait() can hamper entire program optimization, even if never called at runtime (needs evidence).
#if CAN_REVEAL_SURROUNDING
		  auto wait = alwaysWait();
		  auto speed = NormalDuration::speedUpBy(7);
#endif
		  Keyboard::pressKey(Key::SPACE);
	      } else {
		  auto manualMoved = Mouse::cancellableLeftClick();
		  if (manualMoved == Mouse::Moved::YES) {
		      return MoveResult::MANUALLY_CANCELLED;
		  }
	      }
	      updateAfter.emplace_back(boardPos, LookFor::ADJACENT_UNKNOWN);
	      for (auto const& neighbor : fBoard.neighbors(boardPos)) {
		  revealedPositions.insert(neighbor);
	      }
	      break;
	  }
	}
    }
    bool failedToUpdate = false;
    if (!updateAfter.empty()) {
	// move mouse out of the way
	Mouse::realisticMove(boardToNormalPixel({fBoard.width() + 1, fBoard.height() + 1}));
	failedToUpdate = true;
        auto before = now();
	for (auto attempt = 1; failedToUpdate && timeFrom(before).count() < 0.2; ++attempt) {
	    auto screen = Screen::capture();
	    if (!isValidOuterBorder(screen,
				    fOutsideBorder,
				    fOuterBorderMinX,
				    fOuterBorderMaxX,
				    fOuterBorderMinY,
				    fOuterBorderMaxY)) {
		debug::cout << "Try " << attempt << ": couldn't find the board border" << std::endl;
		continue;
	    }
	    failedToUpdate = false;
	    for (auto const& [pos, lookFor] : updateAfter) {
		if (!updateFromScreen(screen, pos, lookFor)) {
		    auto [x, y] = pos;
		    debug::cout << "Try " << attempt << ": failed to update " << x << " " << y << std::endl;
		    failedToUpdate = true;
		    break;
		} else if (lookFor == LookFor::EXACT_SQUARE && fBoard.isMine(pos)) {
		    fOutcome = Outcome::LOST;
		    break;
		}
	    }
	}
    }
    if (failedToUpdate && !isDone()) {
	// May not see the board as done, but may be done:
	// if all unknown squares were marked in this moveset.
	auto const height = fBoard.height();
	auto const width = fBoard.width();
	for (unsigned int i = 0; i < width; ++i) {
	    for (unsigned int j = 0; j < height; ++j) {
		if (fBoard.isUnknown({i, j}) || fBoard.isSafeUnknown({i, j})) {
		    // Found an unknown square.
		    // Give up!
		    debug::cout << "Unknown at " << i << " " << j << std::endl;
		    return MoveResult::MOVED_BUT_NO_CHANGE;
		}
	    }
	}
	// All unknown squares were marked in this moveset
	// so we either won ot lost.
	if (confident) {
	    fOutcome = Outcome::WON;
	} else {
	    fOutcome = Outcome::DONE_UNKNOWN;
	}
    }
    return MoveResult::MOVED;
}

bool ScreenBoard::isDone() const {
    if (fOutcome == Outcome::NOT_DONE && fBoard.isDone()) {
        fOutcome = Outcome::DONE_UNKNOWN;
    }
    return fOutcome != Outcome::NOT_DONE;
}

bool ScreenBoard::won() const {
    return fOutcome == Outcome::WON;
}

bool ScreenBoard::lost() const {
    return fOutcome == Outcome::LOST;
}

void ScreenBoard::print() const {
    fBoard.print();
}

using RectangleSize = std::pair<unsigned int, unsigned int>;

template<class Screen>
static std::map<RectangleSize, std::vector<PixelPosition>> screenRectangles(Screen const& screen) {
    auto const screenHeight = screen.size();
    auto const screenWidth = screen[0].size();
    std::vector topLeftSameColor(screenHeight, std::vector(screenWidth, PixelPosition{0, 0}));
    std::vector bottomRightSameColor(screenHeight, std::vector(screenWidth, PixelPosition{0, 0}));
    for (auto j = 0u; j < screenHeight; ++j) {
	for (auto i = 0u; i < screenWidth; ++i) {
	    PixelPosition topLeft{i, j};
	    auto const matchesLeft = i > 0 && screen[j][i] == screen[j][i - 1];
	    auto const matchesAbove = j > 0 && screen[j][i] == screen[j - 1][i];
	    if (matchesLeft && matchesAbove) {
		if (topLeftSameColor[j][i - 1] == topLeftSameColor[j - 1][i]) {
		    topLeft = topLeftSameColor[j][i - 1];
		}
	    } else if (matchesLeft) {
		if (topLeftSameColor[j][i - 1].y == j) {
		    topLeft = topLeftSameColor[j][i - 1];
		}
	    } else if (matchesAbove) {
		if (topLeftSameColor[j - 1][i].x == i) {
		    topLeft = topLeftSameColor[j - 1][i];
		}
	    }
	    topLeftSameColor[j][i] = topLeft;
	    bottomRightSameColor[topLeft.y][topLeft.x] = {i, j};
	}
    }
    constexpr auto minTileWidth = 10;
    constexpr auto minTileHeight = 10;
    std::map<RectangleSize, std::vector<PixelPosition>> rectangles;
    for (auto j = 0u; j < screenHeight; ++j) {
	for (auto i = 0u; i < screenWidth; ++i) {
	    if (topLeftSameColor[j][i] == PixelPosition{i, j}) {
		auto [botX, botY] = bottomRightSameColor[j][i];
		auto tileWidth = botX - i + 1;
		auto tileHeight = botY - j + 1;
		if (tileWidth >= minTileWidth && tileHeight >= minTileHeight) {
		    rectangles[{tileWidth, tileHeight}].push_back({i, j});
		}
	    }
	}
    }
    return rectangles;
}

static RectangleSize commonDist(std::vector<PixelPosition> const& points) {
    using X = decltype(declval<PixelPosition>().x);
    using Y = decltype(declval<PixelPosition>().y);
    using Count = unsigned int;
    std::map<X, std::set<Y>> columns;
    std::map<Y, std::set<X>> rows;
    for (auto const& [x, y] : points) {
	columns[x].insert(y);
	rows[y].insert(x);
    }
    std::map<X, Count> widthCounts;
    std::map<Y, Count> heightCounts;
    for (auto const& [x, ys] : columns) {
	std::optional<Y> lastY;
	for (Y y : ys) {
	    if (lastY) {
		++heightCounts[y - *lastY];
	    }
	    lastY = y;
	}
    }
    for (auto const& [y, xs] : rows) {
	std::optional<X> lastX;
	for (X x : xs) {
	    if (lastX) {
		++widthCounts[x - *lastX];
	    }
	    lastX = x;
	}
    }
    std::pair<Count, X> bestCommonWidth{0, 0};
    std::pair<Count, Y> bestCommonHeight{0, 0};
    for (auto const& [x, count] : widthCounts) {
	bestCommonWidth = std::max(bestCommonWidth, std::make_pair(count, x));
    }
    for (auto const& [y, count] : heightCounts) {
	bestCommonHeight = std::max(bestCommonWidth, std::make_pair(count, y));
    }
    return {bestCommonWidth.second, bestCommonHeight.second};
}

static std::optional<std::pair<PixelPosition, RectangleSize>> findTopLeftAndSize(
    std::vector<PixelPosition> const& sortedPoints,
    RectangleSize const& distBetween) {
    using Count = unsigned int;
    std::map<PixelPosition, PixelPosition> roots;
    std::map<PixelPosition, Count> numHits;
    auto [dx, dy] = distBetween;
    for (auto const& point : sortedPoints) {
	auto [x, y] = point;
	auto root = point;
	auto const matchesLeft = x >= dx && roots.count({x - dx, y});
	auto const matchesAbove = y >= dy && roots.count({x, y - dy});
	if (matchesLeft && matchesAbove) {
	    if (roots[{x - dx, y}] == roots[{x, y - dy}]) {
		root = roots[{x - dx, y}];
	    }
	} else if (matchesLeft) {
	    root = roots[{x - dx, y}];
	} else if (matchesAbove) {
	    root = roots[{x, y - dy}];
	}
	roots[{x, y}] = root;
	++numHits[root];
    }
    std::pair<Count, PixelPosition> bestTopLeft{0, {0, 0}};
    for (auto [position, count] : numHits) {
	bestTopLeft = std::max(bestTopLeft, std::make_pair(count, position));
    }
    auto topLeft = bestTopLeft.second;
    RectangleSize boardSize{0, 0};
    for (auto point = topLeft; roots.count(point); point.x += dx) {
	++boardSize.first;
    }
    for (auto point = topLeft; roots.count(point); point.y += dy) {
	++boardSize.second;
    }
    for (decltype(boardSize.first) i = 0; i < boardSize.first; ++i) {
	for (decltype(boardSize.second) j = 0; j < boardSize.second; ++j) {
	    auto const& [x, y] = topLeft;
	    if (!roots.count({x + i * dx, y + j * dy})) {
		// check that no squares are missing in the full grid
		return std::nullopt;
	    }
	}
    }
    return std::make_pair(topLeft, boardSize);
}

bool ScreenBoard::isValidOuterBorder(Screen const& screen,
				     Pixel const& expectedValue,
				     unsigned int minX,
				     unsigned int maxX,
				     unsigned int minY,
				     unsigned int maxY) {
    // top and bottom
    for (auto x = minX; x <= maxX; ++x) {
	if (screen.at(x, minY) != expectedValue ||
	    screen.at(x, maxY) != expectedValue) {
	    return false;
	}
    }
    // left and right
    for (auto y = minY; y <= maxY; ++y) {
	if (screen.at(minX, y) != expectedValue ||
	    screen.at(maxX, y) != expectedValue) {
	    return false;
	}
    }
    return true;
}

std::optional<ScreenBoard> ScreenBoard::findFromCurrentScreen() {
    auto screenRaw = Screen::capture();
    auto screen = screenRaw.toLocal();
    auto rectangles = screenRectangles(screen);
    std::vector<PixelPosition> mostFrequentRectangles;
    RectangleSize bestRectSize{0, 0};
    for (auto&& [rectSize, rects] : rectangles) {
	if (rects.size() > mostFrequentRectangles.size()) {
	    std::swap(rects, mostFrequentRectangles);
	    bestRectSize = rectSize;
	}
    }
    auto [tileWidth, tileHeight] = bestRectSize;
    
    constexpr auto minNumTiles = 48;
    if (mostFrequentRectangles.size() < minNumTiles) {
	return std::nullopt;
    }
    auto distBetween = commonDist(mostFrequentRectangles);
    auto topLeftAndSize = findTopLeftAndSize(mostFrequentRectangles, distBetween);
    if (!topLeftAndSize) {
	return std::nullopt;
    }
    auto [topLeft, boardSize] = *topLeftAndSize;
    auto [firstTileXStart, firstTileYStart] = topLeft;
    auto [boardWidth, boardHeight] = boardSize;
    auto [xDistBetween, yDistBetween] = distBetween;
    if (xDistBetween != yDistBetween) {
	return std::nullopt;
    }
    if (boardWidth < 5 || boardHeight < 5) {
	return std::nullopt;
    }
    for (decltype(boardWidth) i = 0; i < boardWidth; ++i) {
	for (decltype(boardHeight) j = 0; j < boardHeight; ++j) {
	    auto eachRectPixel = screen[firstTileYStart + j * yDistBetween][firstTileXStart + i * xDistBetween];
	    auto topLeftRectPixel = screen[firstTileYStart][firstTileXStart];
	    if (eachRectPixel != topLeftRectPixel) {
		// all squares must have the same color
		return std::nullopt;
	    }
	}
    }
    auto lastTileXEnd = firstTileXStart + (boardWidth - 1) * xDistBetween + tileWidth - 1;
    auto lastTileYEnd = firstTileYStart + (boardHeight - 1) * yDistBetween + tileHeight - 1;
    auto lastScreenX = static_cast<decltype(lastTileXEnd)>(screen[0].size() - 1);
    auto lastScreenY = static_cast<decltype(lastTileYEnd)>(screen.size() - 1);
    auto outerBorderMaxWidth = std::min({tileWidth, tileHeight, firstTileXStart, firstTileYStart, lastScreenY - lastTileYEnd, lastScreenX - lastTileXEnd});
    unsigned int outerBorderMinX;
    unsigned int outerBorderMaxX;
    unsigned int outerBorderMinY;
    unsigned int outerBorderMaxY;
    Pixel outerBorderPixel{0, 0, 0};
    for (unsigned int outerBorderDist = 1; outerBorderDist <= outerBorderMaxWidth; ++outerBorderDist) {
	outerBorderMinX = firstTileXStart - outerBorderDist;
	outerBorderMaxX = lastTileXEnd + outerBorderDist;
	outerBorderMinY = firstTileYStart - outerBorderDist;
	outerBorderMaxY = lastTileYEnd + outerBorderDist;
	outerBorderPixel = screen[outerBorderMinY][outerBorderMinX];

	if (isValidOuterBorder(screenRaw, outerBorderPixel, outerBorderMinX, outerBorderMaxX, outerBorderMinY, outerBorderMaxY)) {
	    goto FoundOuterBorder;
	}
    }
    // no border - this could be a board with another window covering part of it
    // Ignore these partial boards.
    return std::nullopt;
FoundOuterBorder:
    auto tileBorderWidth = xDistBetween - tileWidth;
    auto tileBorderHeight = yDistBetween - tileHeight;
    Board board(boardWidth, boardHeight, 10);
    ScreenBoard result(std::move(board),
		       tileWidth,
		       tileHeight,
		       tileBorderWidth,
		       tileBorderHeight,
		       firstTileXStart,
		       firstTileYStart,
		       screenRaw.at(firstTileXStart, firstTileYStart),
		       outerBorderPixel,
		       outerBorderMinX,
		       outerBorderMaxX,
		       outerBorderMinY,
		       outerBorderMaxY);
    return result;
}

static std::string filename() {
    return "tileCache.txt";
}

static void storeTileCacheToFile(ScreenBoard::TileCache const& tileCache) {
    if (tileCache.empty()) {
	return;
    }
    std::ofstream out(filename());
    for (auto const& [data, pixels] : tileCache) {
	out << data << std::endl;
        UL h = pixels.size();
	UL w = pixels[0].size();
	out << h << " " << w << std::endl;
	for (auto const& row : pixels) {
	    for (auto const& pixel : row) {
	        UI r = pixel.r;
		UI g = pixel.g;
		UI b = pixel.b;
		out << r << " " << g << " " << b << std::endl;
	    }
	}
    }
    out << std::flush;
}

static ScreenBoard::TileCache loadTileCacheFromFile() {
    ScreenBoard::TileCache cache;
    try {
	std::ifstream in(filename());
	while (in) {
	    Board::Data data;
	    in >> data;
	    UL h;
	    UL w;
	    in >> h >> w;
	    if (in) {
		std::vector pixels(h, std::vector(w, Pixel{0, 0, 0}));
		for (UI y = 0; y < h; ++y) {
		    for (UI x = 0; x < w; ++x) {
			UI r;
			UI g;
			UI b;
			in >> r >> g >> b;
			pixels[y][x] = {static_cast<UC>(r), static_cast<UC>(g), static_cast<UC>(b)};
		    }
		}
		if (in) {
		    cache.insert({data, std::move(pixels)});
		}
	    }
	}
    } catch (...) {
    }
    return cache;
}

ScreenBoard::ScreenBoard(Board board,
			 unsigned int tileWidth,
			 unsigned int tileHeight,
			 unsigned int tileBorderWidth,
			 unsigned int tileBorderHeight,
			 unsigned int firstTileXStart,
			 unsigned int firstTileYStart,
			 Pixel blankPixel,
			 Pixel outsideBorder,
			 unsigned int outerBorderMinX,
			 unsigned int outerBorderMaxX,
			 unsigned int outerBorderMinY,
			 unsigned int outerBorderMaxY)
    : fBoard(std::move(board))
    , fTileWidth(tileWidth)
    , fTileHeight(tileHeight)
    , fTileBorderWidth(tileBorderWidth)
    , fTileBorderHeight(tileBorderHeight)
    , fFirstTileXStart(firstTileXStart)
    , fFirstTileYStart(firstTileYStart)
    , fBlankPixel(blankPixel)
    , fOutsideBorder(outsideBorder)
    , fOuterBorderMinX(outerBorderMinX)
    , fOuterBorderMaxX(outerBorderMaxX)
    , fOuterBorderMinY(outerBorderMinY)
    , fOuterBorderMaxY(outerBorderMaxY)
    , fUsesSpaceForSurrounding(true)
    , fGuessWhenUnsure(true)
    , fTileCache(loadTileCacheFromFile()) {
}

ScreenBoard::~ScreenBoard() {
    try {
	if (fTileCacheChanged) {
	    storeTileCacheToFile(fTileCache);
	}
    } catch (...) {
	// never throw from destructor
    }
}

bool ScreenBoard::updateFromScreen(Screen const& screen, BoardPosition const& position, LookFor lookFor) {
    switch (lookFor) {
      case LookFor::ADJACENT_UNKNOWN: {
	  for (auto [i, j] : fBoard.neighbors(position)) {
	      if (fBoard.isUnknown({i, j}) || fBoard.isSafeUnknown({i, j})) {
		  if (!updateFromScreen(screen, {i, j}, LookFor::EXACT_SQUARE)) {
		      return false;
		  }
	      }
	  }
	  return true;
      }
      case LookFor::EXACT_SQUARE: {
	  auto [x, y] = position;
	  auto minX = fFirstTileXStart + x * (fTileWidth + fTileBorderWidth);
	  auto maxX = minX + fTileWidth - 1;
	  auto minY = fFirstTileYStart + y * (fTileHeight + fTileBorderHeight);
	  auto maxY = minY + fTileHeight - 1;
	  bool allSameColor = true;
	  auto topLeftPixel = screen.at(minX, minY);
	  for (decltype(minX) x = minX; x <= maxX; ++x) {
	      for (decltype(minY) y = minY; y <= maxY; ++y) {
		  if (screen.at(x, y) != topLeftPixel) {
		      allSameColor = false;
		      goto AfterSameColorLoop;
		  }
	      }
	  }
	AfterSameColorLoop:
	  Board::Data value;
	  if (allSameColor) {
	      if (topLeftPixel == fBlankPixel && screen.at(minX - 1, minY - 1) != fBlankPixel) {
		  // Still blank? Should have found a value.
		  return false;
	      } else {
		  // most minesweeper boards show '0' as an empty space
		  // but different from an unclicked space
		  value = '0';
	      }
	  } else {
	      std::vector tileData(fTileHeight, std::vector(fTileWidth, Pixel{0, 0, 0}));
	      for (decltype(fTileHeight) j = 0; j < fTileHeight; ++j) {
		  auto&& row = tileData[j];
		  for (decltype(fTileWidth) i = 0; i < fTileWidth; ++i) {
		      row[i] = screen.at(minX + i, minY + j);
		  }
	      }
	      for (auto const& [cacheValue, cacheTile] : fTileCache) {
		  if (cacheTile == tileData) {
		      value = cacheValue;
		      goto FoundCacheTile;
		  }
	      }
	      std::cout << "I require assistance with " << position.x << " " << position.y << std::endl;
	      Mouse::realisticMove(boardToNormalPixel(position));
	      {
		  auto holdingApple = Keyboard::holdKey(Key::APPLE);
		  Keyboard::pressKey(Key::TAB);
	      }
	      
	      std::cin >> value;
	      Mouse::realisticMove(boardToNormalPixel(position));
	      Mouse::leftClick(); // get back to board
	      fTileCache.insert({value, std::move(tileData)});
	      fTileCacheChanged = true;
	  FoundCacheTile:;
	  }
	  fBoard.mark(position, value);
	  if (value == '0') {
	      if (!updateFromScreen(screen, position, LookFor::ADJACENT_UNKNOWN)) {
		  return false;
	      }
	  }
	  return true;
      }
    }
}

Mouse::Position ScreenBoard::boardToNormalPixel(BoardPosition const& boardPosition) const {
    auto xStart = fFirstTileXStart + boardPosition.x * (fTileWidth + fTileBorderWidth);
    auto yStart = fFirstTileYStart + boardPosition.y * (fTileHeight + fTileBorderHeight);
    auto xSpread = (fTileWidth - 1) / 2.0;
    auto ySpread = (fTileHeight - 1) / 2.0;
    auto xDiff = 0.0;
    while (xDiff <= 0 || xDiff >= fTileWidth) {
	xDiff = randNormal(xSpread, xSpread / 2.0);
    }
    auto yDiff = 0.0;
    while (yDiff <= 0 || yDiff >= fTileHeight) {
	yDiff = randNormal(ySpread, ySpread / 2.0);
    }
    auto x = xStart + xDiff;
    auto y = yStart + yDiff;
    // Mouse position is half of pixels. Why? Ask Apple.
    return {x / 2, y / 2};
}

bool ScreenBoard::moveToBoardPosition(BoardPosition const& boardPosition) const {
    auto [x, y] = boardToNormalPixel(boardPosition);
    return Mouse::realisticMove({x, y}) == Mouse::Moved::NO;
}

