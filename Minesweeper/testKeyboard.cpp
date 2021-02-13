#include "Keyboard.hpp"

#include "../TypeUtils.hpp"
#include "../Wait.hpp"

SI main(Argc const, Argv const) {
    Time halfSecond(0.5);
    /*
    {
	auto holdingX = Keyboard::holdKey(Key::X);
	waitFor(halfSecond); // repeating x
	{
	    auto holdingShift = Keyboard::holdKey(Key::SHIFT);
	    waitFor(halfSecond); // repeating X
	}
	waitFor(halfSecond); // repeating x
	{
	    auto holdingShift = Keyboard::holdKey(Key::SHIFT);
	    waitFor(halfSecond); // repeating X
	    auto holdingA = Keyboard::holdKey(Key::A);
	    waitFor(halfSecond); // repeating A
	    holdingShift.reset(); // release shift
	    waitFor(halfSecond); // repeating a
	    Keyboard::pressKey(Key::B); // single b - should stop repeating a
	    waitFor(halfSecond); // should do nothing after the b
	}
	waitFor(halfSecond); // should do nothing after the b
    }
    Keyboard::typeText("dank MEMEs123!\n");
    Keyboard::typeText("./run.me\n");
    // */

    //*
    {
	auto holdingApple = Keyboard::holdKey(Key::APPLE);
	Keyboard::pressKey(Key::TAB);
	//waitFor(Time(1));
	Keyboard::pressKey(Key::TAB);
	//waitFor(halfSecond);
    }
    
    Keyboard::pressKey(Key::A);
    {
	auto holdingShift = Keyboard::holdKey(Key::SHIFT);
	Keyboard::pressKey(Key::B);
    }
    Keyboard::pressKey(Key::C);
    Keyboard::pressKey(Key::RETURN);
    //*/
    return 0;
}
