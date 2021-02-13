#include "MouseController.hpp"
#include "../TypeUtils.hpp"
#include "Keyboard.hpp"

#include <iostream>

SI main(Argc const, Argv const) {
      Mouse::immediateMoveWithEvent(200, 200);
      Mouse::leftClick();
      do {
	  auto manualMove = Mouse::realisticMove(900, 200, std::chrono::seconds(1));
      std::cout << (manualMove == Mouse::Moved::YES ? "moved" : "not moved") << std::endl;
      }
      while (Mouse::leftClick(std::chrono::seconds(2)) == Mouse::Moved::YES);
    /*
    {
	auto holdingCtrl = Keyboard::holdKey(Key::CONTROL);
	Mouse::leftClick();
    }
    auto [x,y] = Mouse::position();
    Mouse::realisticMove(x + 50, y + 10);
    Mouse::leftClick();
    {
	auto holdingApple = Keyboard::holdKey(Key::RIGHT_APPLE);
	Keyboard::pressKey(Key::V);
    }
    */
    return 0;
}
