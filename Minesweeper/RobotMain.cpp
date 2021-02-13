#include "Board.hpp"
#include "../Now.hpp"
#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

[[noreturn]] static void dieWith(std::string const& message) {
    throw std::runtime_error(message);
}

[[noreturn]] static void noBoard(std::string const& timeout) {
    dieWith("No board found within " + timeout + ".");
}

[[noreturn]] static void noMove() {
    dieWith("No move found.");
}

[[noreturn]] static void manualMove() {
    dieWith("Mouse manually moved.");
}

[[noreturn]] static void noBoardChange() {
    dieWith("No board change after automatic move.");
}

template<class Speed>
static SI mainn(Speed&& speed) {
    auto timeout = std::chrono::seconds(10);
    auto time = "10 seconds";
    auto reactionTime = Time(0.2);
    while (true) {
	std::cout << "Looking for clean board for " << time << std::endl;
	std::optional<ScreenBoard> screenBoard = ScreenBoard::findFromScreen(timeout);
	auto mousePositionBeforeGame = Mouse::position();
	if (!screenBoard) {
	    noBoard(time);
	}

	ScreenBoard& board = *screenBoard;
	auto lastMousePos = mousePositionBeforeGame;
	bool isFirstMove = true;
	while (true) {
	    auto fast = speed(); (void)fast;
	    board.print();
	    if (lastMousePos != Mouse::position()) {
		manualMove();
	    }
	    auto beforeMove = now();
	    auto moveResult = board.doNextMove();
	    lastMousePos = Mouse::position();
	    switch (moveResult) {
	      case ScreenBoard::MoveResult::NONE_AVAILABLE:
		noMove();
	      case ScreenBoard::MoveResult::MANUALLY_CANCELLED:
		manualMove();
	      case ScreenBoard::MoveResult::MOVED:
		break;
	      case ScreenBoard::MoveResult::MOVED_BUT_NO_CHANGE:
		if (!isFirstMove) {
		    // The first move may have switched control
		    // from another program to the one with minesweeper
		    // in which case the first click had no change in board.
		    // In this case, just ignore and move again.
		    noBoardChange();
		}
	    }
	    if (board.isDone()) {
		break;
	    }
	    auto timeSoFar = timeFrom(beforeMove);
	    if (timeSoFar < reactionTime) {
		waitFor(reactionTime - timeSoFar);
	    }
	    isFirstMove = false;
	}
	if (board.won()) {
	    std::cout << "Won! Finished board:" << std::endl;
	} else if (board.lost()) {
	    std::cout << "Lost :( board:" << std::endl;
	} else {
	    std::cout << "Finished board (unsure if win or loss since squares didn't update):" << std::endl;
	}
	board.print();
        Mouse::realisticMove(mousePositionBeforeGame);
	waitFor(reactionTime);
	if (board.lost()) {
	    std::cout << "Starting new game in 2s. Move mouse to cancel." << std::endl;
	    waitFor(Time(2));
	    if (Mouse::position() == mousePositionBeforeGame) {
		Mouse::leftClick();
	    }
	}
    }
}

SI main(Argc const argc, Argv const argv) {
    try {
	// so we call destructors during stack unwinding
        
	if (argc == 2) {
	    if (double x = atof(argv[1])) {
		return mainn([x]() { return NormalDuration::speedUpBy(x); });
	    } else {
		return mainn([]() { return neverWait(); });
	    }
	}
	return mainn([]() { return 0; });
    } catch (...) {
	throw;
    }
}
