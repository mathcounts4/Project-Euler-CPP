#pragma once

#include "Key.hpp"
#include "../VirtualDestructor.hpp"
#include "../Wait.hpp"

#include <memory>
#include <string>

class Keyboard {
  public:
    static constexpr NormalDuration DEFAULT_KEY_WAIT = NormalDuration(std::chrono::milliseconds(10 /* 30 */),
								      std::chrono::milliseconds(0 /* 5 */));
    static constexpr NormalDuration DEFAULT_DELAY_BEFORE_HOLD_REPEAT = NormalDuration(std::chrono::milliseconds(300),
										      std::chrono::milliseconds(30));
    static constexpr NormalDuration DEFAULT_DELAY_BETWEEN_REPEATS = NormalDuration(std::chrono::milliseconds(80),
										   std::chrono::milliseconds(8));
    static CGEventFlags currentModifierFlags();
    
    // If you don't wait after, modifier keys like SHIFT might not be processed in time,
    // or might be considered "held" on the next key press (after SHIFT released)
    static void pressKey(Key key,
			 NormalDuration const& timeDown = DEFAULT_KEY_WAIT,
			 NormalDuration const& timeAfter = DEFAULT_KEY_WAIT);
    using KeyRelease = std::unique_ptr<VirtualDestructor>;
    // Note: holdKey does not block the current thread at all
    // The callsite must judge if it should wait
    [[nodiscard]] static KeyRelease holdKey(Key key,
					    NormalDuration const& delayAfterDownThisThread = DEFAULT_KEY_WAIT,
					    NormalDuration const& delayBeforeFirstRepeatOtherThread = DEFAULT_DELAY_BEFORE_HOLD_REPEAT,
					    NormalDuration const& timeBetweenRepeatsOtherThread = DEFAULT_DELAY_BETWEEN_REPEATS,
					    NormalDuration const& timeAfterRelease = DEFAULT_KEY_WAIT);
    [[nodiscard]] static bool isModifier(Key key);

    // Skips un-typeable characters.
    // For both carriage return and new line use the "return" key
    static void typeText(std::string const& text,
			 NormalDuration const& timeDownPerKey = DEFAULT_KEY_WAIT,
			 NormalDuration const& timeAfterPerKey = DEFAULT_KEY_WAIT);
};
