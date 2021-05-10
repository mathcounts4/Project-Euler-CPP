#pragma once

#include "MouseController.hpp"
#include "View.hpp"
#include "../MyHashUtil.hpp"
#include "../Optional.hpp"
#include "../Wait.hpp"

#include <unordered_map>
#include <utility>
#include <vector>

class Screen;

struct PixelPosition {
    unsigned int x;
    unsigned int y;
    friend inline bool operator==(PixelPosition const& p1, PixelPosition const& p2) {
	return p1.x == p2.x && p1.y == p2.y;
    }
    friend inline bool operator<(PixelPosition const& p1, PixelPosition const& p2) {
	if (p1.y != p2.y) {
	    return p1.y < p2.y;
	}
	return p1.x < p2.x;
    }
};
MAKE_HASHABLE(PixelPosition, p, p.x, p.y);

struct BoardPosition {
    unsigned int x;
    unsigned int y;
    friend inline bool operator==(BoardPosition const& p1, BoardPosition const& p2) {
	return p1.x == p2.x && p1.y == p2.y;
    }
    friend inline bool operator<(BoardPosition const& p1, BoardPosition const& p2) {
	if (p1.y != p2.y) {
	    return p1.y < p2.y;
	}
	return p1.x < p2.x;
    }
};
MAKE_HASHABLE(BoardPosition, p, p.x, p.y);

class Board {
  public:
    using Data = char;
    enum class Mark { REVEAL, MINE, REVEAL_SURROUNDING };
    using Move = std::pair<BoardPosition, Mark>;

  public:
    static constexpr Data UNKNOWN = ' ';
    static constexpr Data MINE = '*';
    static constexpr Data EXPLODED_MINE = 'X';
    static constexpr Data SAFE_UNKNOWN = '#';
    
  public:
    Board(unsigned int width, unsigned int height, unsigned int mines);

    std::vector<Move> nextCorrectMoves();
    Optional<Move> nextGuessMove() const;
    std::vector<BoardPosition> neighbors(BoardPosition const& position) const;
    void print() const;
    unsigned int width() const {
	return fWidth;
    }
    unsigned int height() const {
	return fHeight;
    }
    bool isDone() const;
    bool isUnknown(BoardPosition const& position) const;
    bool isSafeUnknown(BoardPosition const& position) const;
    bool isMine(BoardPosition const& position) const;

    void mark(BoardPosition const& position, Data value);

  private:
    struct AdjInfo {
	unsigned int fNumUnknown;
	unsigned int fNumMines;
    };
    AdjInfo adj(BoardPosition const& pos) const;
    std::vector<Move> movesFromTryingPossibilities(BoardPosition const& pos, unsigned int numUnknownMines) const;
    Data getData(BoardPosition const& position) const;
    bool hasValue(BoardPosition const& position, Data value) const;
    
  private:
    unsigned int const fWidth;
    unsigned int const fHeight;
    std::vector<std::vector<Data>> fTiles;
    bool fCanRevealSurrounding;
    unsigned int fMinesLeft;
};

class ScreenBoard {
  public:
    enum class MoveResult { MOVED, NONE_AVAILABLE, MANUALLY_CANCELLED, MOVED_BUT_NO_CHANGE };
  public:
    static std::optional<ScreenBoard> findFromScreen(Time maxTime,
						     Time delayBetween = std::chrono::milliseconds(250));
    MoveResult doNextMove();
    bool isDone() const;
    bool won() const;
    bool lost() const;
    void print() const;
    using TileCache = std::unordered_multimap<Board::Data, std::vector<std::vector<Pixel>>>;
    ScreenBoard(ScreenBoard&&) = default;
    ~ScreenBoard();

  private:
    static bool isValidOuterBorder(Screen const& screen,
				   Pixel const& expectedValue,
				   unsigned int minX,
				   unsigned int maxX,
				   unsigned int minY,
				   unsigned int maxY);
    static std::optional<ScreenBoard> findFromCurrentScreen();
    ScreenBoard(Board board,
		unsigned int tileWidth,
		unsigned int tileHeight,
		unsigned int tileBorderWidth,
		unsigned int tileBorderHeight,
		unsigned int firstTileXStart,
		unsigned int firstTileYStart,
		Pixel blankPixel,
		Pixel outerBorder,
		unsigned int outerBorderMinX,
		unsigned int outerBorderMaxX,
		unsigned int outerBorderMinY,
		unsigned int outerBorderMaxY);
    enum class Outcome { NOT_DONE, DONE_UNKNOWN, LOST, WON };
    enum class LookFor { EXACT_SQUARE, ADJACENT_UNKNOWN };
    bool updateFromScreen(Screen const& screen, BoardPosition const& position, LookFor lookFor);
    Mouse::Position boardToNormalPixel(BoardPosition const& boardPosition) const;
    bool moveToBoardPosition(BoardPosition const& boardPosition) const;

  private:
    Board fBoard;
    unsigned int fTileWidth;
    unsigned int fTileHeight;
    unsigned int fTileBorderWidth;
    unsigned int fTileBorderHeight;
    unsigned int fFirstTileXStart;
    unsigned int fFirstTileYStart;
    Pixel fBlankPixel;
    Pixel fOutsideBorder;
    unsigned int fOuterBorderMinX;
    unsigned int fOuterBorderMaxX;
    unsigned int fOuterBorderMinY;
    unsigned int fOuterBorderMaxY;
    bool fUsesSpaceForSurrounding;
    bool fGuessWhenUnsure;
    mutable Outcome fOutcome{Outcome::NOT_DONE};
    TileCache fTileCache{};
    bool fTileCacheChanged{false};
};
