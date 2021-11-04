#include "Board.hpp"
#include "Keyboard.hpp"
#include "../DebugPrint.hpp"
#include "../Now.hpp"
#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

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
	    std::cout << "No board found within " << time << "." << std::endl;
	    continue;
	}

	ScreenBoard& board = *screenBoard;
	auto lastMousePos = mousePositionBeforeGame;
	Optional<void> ok;
	bool manualCancel = false;
	for (bool isFirstMove = true; true; isFirstMove = false) {
	    auto fast = speed(); (void)fast;
	    if (debug::doPrint) {
		board.print();
	    }
	    if (lastMousePos != Mouse::position()) {
		manualCancel = true;
		ok = Failure("Mouse manually moved.");
		break;
	    }
	    auto beforeMove = now();
	    auto moveResult = board.doNextMove();
	    lastMousePos = Mouse::position();
	    switch (moveResult) {
	      case ScreenBoard::MoveResult::NONE_AVAILABLE:
		ok = Failure("No move found.");
		break;
	      case ScreenBoard::MoveResult::MANUALLY_CANCELLED:
		manualCancel = true;
		ok = Failure("Mouse manually moved.");
		break;
	      case ScreenBoard::MoveResult::MOVED:
		break;
	      case ScreenBoard::MoveResult::MOVED_BUT_NO_CHANGE:
		if (!isFirstMove) {
		    // The first move may have switched control
		    // from another program to the one with minesweeper
		    // in which case the first click had no change in board.
		    // In this case, just ignore and move again.
		    ok = Failure("No board change after automatic move.");
		}
		break;
	    }
	    if (board.isDone() || !ok) {
		break;
	    }
	    auto timeSoFar = timeFrom(beforeMove);
	    if (timeSoFar < reactionTime) {
		waitFor(reactionTime - timeSoFar);
	    }
	}
	if (board.won()) {
	    std::cout << "Won!" << std::endl;
	} else if (board.lost()) {
	    std::cout << "Lost :(" << std::endl;
	} else if (ok) {
	    std::cout << "Finished but unsure of result." << std::endl;
	} else {
	    std::cout << ok.cause() << std::endl;
	}
	std::cout << "Final board:" << std::endl;
	board.print();
	if (!manualCancel && !board.lost()) {
	    auto const currentPos = Mouse::position();
	    std::cout << "Pressing [Return] in 5 seconds. Move mouse to cancel." << std::endl;
	    waitFor(Time(5));
	    if (currentPos == Mouse::position()) {
		// In case a "won" popup is here:
		Keyboard::pressKey(Key::RETURN);
	    } else {
		manualCancel = true;
	    }
	}
	if (!manualCancel) {
	    Mouse::realisticMove(mousePositionBeforeGame);
	    waitFor(reactionTime);
	    std::cout << "Clicking to start a new game in 2s. Move mouse to cancel." << std::endl;
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
