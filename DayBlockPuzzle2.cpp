/*

https://www.facebook.com/permalink.php?story_fbid=141921821507093&id=103236355375640

https://filmmings.com/products/daily-calendar-puzzle?fbclid=IwAR3xdoyevxm8layWTGTwvK9SssynuR0VypPEYKNTwQg0gADtvHWaVEbYsvA

<DayBlockPuzzleBig.jpg>
<DayBlockPuzzleBigSolved.webp>

We have a board with a bunch of squares to be filled in by a set of pieces.
The board looks like:

+-----+-----+-----+-----+-----+-----+
| Jan | Feb | Mar | Apr | May | Jun |
+-----+-----+-----+-----+-----+-----+
| Jul | Aug | Sep | Oct | Nov | Dec |
+-----+-----+-----+-----+-----+-----+-----+
|  1  |  2  |  3  |  4  |  5  |  6  |  7  |
+-----+-----+-----+-----+-----+-----+-----+
|  8  |  9  | 10  | 11  | 12  | 13  | 14  |
+-----+-----+-----+-----+-----+-----+-----+
| 15  | 16  | 17  | 18  | 19  | 20  | 21  |
+-----+-----+-----+-----+-----+-----+-----+
| 22  | 23  | 24  | 25  | 26  | 27  | 28  |
+-----+-----+-----+-----+-----+-----+-----+
| 29  | 30  | 31  | Sun | Mon | Tue | Wed |
+-----+-----+-----+-----+-----+-----+-----+
                        | Thu | Fri | Sat |
                        +-----+-----+-----+

And the pieces are shaped as:

*/

#include <string_view>
constexpr std::string_view piecesRaw = R"(

XXX
X X

XXX
XX

XXX
  XX

XXXX
X

XXX
X
X

X
XXX
  X

XXX
 X
 X

XXXX

XX
 XX

XXX
X

)";

/*

The puzzle is to, for each valid day of the year and day of the week, to arrange the pieces so that only that month, day, and weekday are not covered.

We see that there are 12 + 31 + 7 = 50 squares, and the pieces cover 5*7+4*3 = 47 squares, so the pieces cannot overlap.

*/

#include "DebugPrint.hpp"
#include "O_Set.hpp"
#include "Point.hpp"
#include "Vec.hpp"

#include <cctype>

using Index = UI;
using Point = Point2D<Index>;
using Piece = O_Set<Point>;

static auto const pieces = []() {
    O_Set<Point> allPoints;
    Index row = 0;
    Index col = 0;
    for (char c : piecesRaw) {
	if (c == '\n') {
	    ++row;
	    col = 0;
	} else {
	    ++col;
	}
	if (!std::isspace(c)) {
	    allPoints.insert({row, col});
	}
    }
    Vec<Piece> allPieces;
    while (!allPoints.empty()) {
	Piece thisPiece;
        Vec<Point> worklist;
	auto addToWorklist = [&allPoints, &worklist, &thisPiece](Point p) {
	    auto it = allPoints.find(p);
	    if (it == allPoints.end()) {
		return;
	    }
	    allPoints.erase(it);
	    thisPiece.emplace(p);
	    worklist.push_back(p);
	};
        addToWorklist(*allPoints.begin());
	while (!worklist.empty()) {
	    auto [x, y] = worklist.back();
	    worklist.pop_back();
	    for (auto adj : Vec<Point>{{x+1,y}, {x-1,y}, {x,y+1}, {x,y-1}}) {
		addToWorklist(adj);
	    }
	}
	
	auto minRow = std::min_element(thisPiece.begin(), thisPiece.end(), [](Point const& a, Point const& b) { return a.x < b.x; })->x;
	auto minCol = std::min_element(thisPiece.begin(), thisPiece.end(), [](Point const& a, Point const& b) { return a.y < b.y; })->y;
	Piece thisPieceShifted;
	for (auto [x, y] : thisPiece) {
	    thisPieceShifted.insert({x - minRow, y - minCol});
	}
	allPieces.emplace_back(std::move(thisPieceShifted));
    }
    return allPieces;
}();

static Piece flip(Piece const& piece) {
    auto maxCol = std::max_element(piece.begin(), piece.end(), [](Point const& a, Point const& b) { return a.y < b.y; })->y;
    Piece result;
    for (auto [x, y] : piece) {
	result.insert({x, maxCol - y});
    }
    return result;
}

static Piece rotate(Piece const& piece) {
    // (x,y) -> (y, max_x-x)
    auto maxRow = std::max_element(piece.begin(), piece.end(), [](Point const& a, Point const& b) { return a.x < b.x; })->x;
    Piece result;
    for (auto [x, y] : piece) {
	result.insert({y, maxRow - x});
    }
    return result;
}

static auto const piecesWithRotationAndReflection = []() {
    Vec<Vec<Piece>> all;
    for (Piece piece: pieces) {
	O_Set<Piece> currentPiece;
	for (UI flips = 0; flips < 2; ++flips, piece = flip(piece)) {
	    for (UI rot = 0; rot < 4; ++rot, piece = rotate(piece)) {
		currentPiece.insert(piece);
	    }
	}
	all.emplace_back(currentPiece.begin(), currentPiece.end());
    }
    return all;
}();

struct Board {
  public:
    friend struct ScopedPlacedPiece;
    struct ScopedPlacedPiece {
	Board& fBoard;
	Piece const& fPiece;
	Index const fDx;
	Index const fDy;
	
	ScopedPlacedPiece(Board& board, Piece const& piece, Index dx, Index dy)
	    : fBoard(board)
	    , fPiece(std::move(piece))
	    , fDx(dx)
	    , fDy(dy) {
	    auto numPieces = ++fBoard.fNumPieces;
	    for (auto const& [x, y] : fPiece) {
		fBoard.mark({x + fDx, y + fDy}, numPieces);
	    }
	}
	~ScopedPlacedPiece() {
	    for (auto const& [x, y] : fPiece) {
		fBoard.mark({x + fDx, y + fDy}, 0);
	    }
	    --fBoard.fNumPieces;
	}
    };

  public:
    // 1-based month, 1-based day, 1-based weekday (1 = Sun)
    Board(UI month, UI day, UI weekday)
	: fNumPieces(0) {
	SZ numMonths = 12;
	SZ monthsPerRow = 6;
	SZ numDays = 31;
	SZ daysPerRow = 7;
	SZ numWeekdays = 7;
	SZ weekdaysPerRow = 7; // max row width
	if (month < 1 || month > numMonths) {
	    throw_exception<std::logic_error>("Month out of range: " + to_string(month));
	}
	if (day < 1 || day > numDays) {
	    throw_exception<std::logic_error>("Day out of range: " + to_string(day));
	}
	if (weekday < 1 || weekday > numWeekdays) {
	    throw_exception<std::logic_error>("Weekday out of range: " + to_string(weekday));
	}
	B monthMarked = false;
	for (SZ monthsLeft = numMonths; monthsLeft; ) {
	    SZ monthsInRow = std::min(monthsLeft, monthsPerRow);
	    fMarked.emplace_back(Vec<UI>(monthsInRow, 0));
	    if (monthMarked) {
	    } else if (month <= monthsInRow) {
		fMarked.back()[month - 1] = 100;
		monthMarked = true;
	    } else {
		month -= monthsInRow;
	    }
	    monthsLeft -= monthsInRow;
	}
	B dayMarked = false;
	for (SZ daysLeft = numDays; daysLeft; ) {
	    SZ daysInRow = std::min(daysLeft, daysPerRow);
	    fMarked.emplace_back(Vec<UI>(daysInRow, 0));
	    if (dayMarked) {
	    } else if (day <= daysInRow) {
		fMarked.back()[day - 1] = 101;
		dayMarked = true;
	    } else {
		day -= daysInRow;
	    }
	    daysLeft -= daysInRow;
	}
	B weekdayMarked = false;
	for (SZ weekdaysLeft = numWeekdays; weekdaysLeft; ) {
	    // weekdays are different:
	    // rather than starting on a new row, they continue on the last one
	    // and they are right-justified
	    if (auto lastRowSize = fMarked.back().size(); lastRowSize < weekdaysPerRow) {
		auto weekdaysToAdd = std::min(weekdaysLeft, weekdaysPerRow - lastRowSize);
		fMarked.back().resize(lastRowSize + weekdaysToAdd, 0);
		if (weekday <= weekdaysToAdd) {
		    fMarked.back()[lastRowSize + weekday - 1] = 102;
		    weekdayMarked = true;
		} else {
		    weekday -= weekdaysToAdd;
		}
		weekdaysLeft -= weekdaysToAdd;
	    } else {
		auto weekdaysToAdd = std::min(weekdaysLeft, weekdaysPerRow);
		auto buffer = weekdaysPerRow - weekdaysToAdd;
		fMarked.emplace_back(Vec<UI>(buffer, 99)); // unused
		fMarked.back().resize(weekdaysPerRow, 0);
		if (weekdayMarked) {
		} else if (weekday <= weekdaysToAdd) {
		    fMarked.back()[buffer + weekday - 1] = 102;
		    weekdayMarked = true;
		} else {
		    weekday -= weekdaysToAdd;
		}
		weekdaysLeft -= weekdaysToAdd;
	    }
	}
	// weekdays are TODO
    }
    [[nodiscard]] Optional<ScopedPlacedPiece> scopedPlacePiece(Piece const& piece, Index dx, Index dy) {
	for (auto const& [px, py] : piece) {
	    auto x = px + dx;
	    auto y = py + dy;
	    if (x >= fMarked.size() || y >= fMarked[x].size()) {
		return Failure("Out of bounds");
	    }
	    if (fMarked[x][y]) {
		return Failure("Point already used");
	    }
	}
	return {*this, piece, dx, dy};
    }
    
    void mark(Point const& point, UI markedIndex) {
	auto [x, y] = point;
	fMarked[x][y] = markedIndex;
    }

    Point firstMissing() const {
	for (SZ i = 0, m = fMarked.size(); i < m; ++i) {
	    auto const& row = fMarked[i];
	    for (SZ j = 0, n = row.size(); j < n; ++j) {
		if (!row[j]) {
		    return {static_cast<UI>(i), static_cast<UI>(j)};
		}
	    }
	}
        throw_exception<std::logic_error>("No missing locations:\n" + to_string(*this));
    }

  private:
    friend struct Class<Board>;

  private:
    Vec<Vec<UI>> fMarked;
    UI fNumPieces;
};
template<>
struct Class<Board> {
    using T = Board;
    static std::ostream& print(std::ostream& os, T const& t) {
	auto const& marks = t.fMarked;
	for (SZ x = 0, m = marks.size(); x < m; ++x) {
	    auto const& row = marks[x];
	    for (SZ y = 0, n = row.size(); y < n; ++y) {
		UI piece = row[y];
		// up: 1, down: 2, left: 4, right: 8
		constexpr char const* connectors[] = {
		    "▪", "╹", "╻", "┃",
		    "╸", "┛", "┓", "┫",
		    "╺", "┗", "┏", "┣",
		    "━", "┻", "┳", "╋",
		};
		// see which adj belong to the same piece
		B left = y > 0 && row[y-1] == piece;
		B right = y+1 < n && row[y+1] == piece;
		B up = x > 0 && y < marks[x-1].size() && marks[x-1][y] == piece;
		B down = x+1 < m && y < marks[x+1].size() && marks[x+1][y] == piece;
		UI dispIdx = 0;
		if (left) {
		    dispIdx += 4;
		}
		if (right) {
		    dispIdx += 8;
		}
		if (up) {
		    dispIdx += 1;
		}
		if (down) {
		    dispIdx += 2;
		}
		auto background = '4'; // blue
	        auto toDisp = connectors[dispIdx];
		if (dispIdx == 0) {
		    background = '2'; // green
		}
		if (piece == 0) {
		    background = '1'; // red
		    toDisp = " ";
		}
		if (piece == 99) {
		    background = '3'; // yellow
		    toDisp = " ";
		}
	        os << "\x1b[4" << background << "m" << toDisp;
	    }
	    os << "\x1b[0m" << std::endl;
	}
	return os;
    }
};

static UI place(Board& board, Vec<B>& piecesUsed) {
    if (std::all_of(piecesUsed.begin(), piecesUsed.end(), [](auto x) { return x; })) {
	debug::cout << board << std::endl;
	// success!
	return 1;
    }

    UI total = 0;
    auto [x, y] = board.firstMissing();
    for (SZ i = 0, n = piecesWithRotationAndReflection.size(); i < n; ++i) {
	if (!piecesUsed[i]) {
	    piecesUsed[i] = true;
	    for (auto const& piece : piecesWithRotationAndReflection[i]) {
		auto py = piece.begin()->y; // shift first location in piece to (x,y). First location is always (0,py).
		if (py <= y) {
		    if (auto placed = board.scopedPlacePiece(piece, x, y - py)) {
		        total += place(board, piecesUsed);
			if (debug::doPrint && total) {
			    return total;
			}
		    }
		}
	    }
	    piecesUsed[i] = false;
	}
    }
    return total;
}

static void doit(UI month, UI day, UI weekday, UI& minSlns) {
    constexpr char const* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    constexpr char const* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    Board board(month, day, weekday);
    auto numPieces = piecesWithRotationAndReflection.size();
    Vec<B> piecesUsed(numPieces, false);
    std::cout << weekdays[weekday-1] << " " << months[month-1] << " " << day;
    debug::cout << std::endl;
    auto success = place(board, piecesUsed);
    if (success) {
	if (!debug::doPrint) {
	    if (success <= minSlns) {
		std::cout << " (new min)";
		minSlns = success;
	    }
	    std::cout << ": " << success << " solutions" << std::endl;
	}
    } else {
	std::cout << " seems impossible:" << std::endl;
	std::cout << board << std::endl;
    }
}

SI main(Argc const argc, Argv const) {
    if (argc == 2) {
	for (auto const& set : piecesWithRotationAndReflection) {
	    for (auto const& element : set) {
		std::cout << element << std::endl;
	    }
	    std::cout << std::endl;
	}
	return 0;
    }
    UI minSlns = 1e9;
    UI daysPerMonth[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (UI month = 1; month <= 12; ++month) {
	UI days = daysPerMonth[month-1];
	for (UI day = 1; day <= days; ++day) {
	    for (UI weekday = 1; weekday <= 7; ++weekday) {
		doit(month, day, weekday, minSlns);
	    }
	}
    }
    return 0;
}


