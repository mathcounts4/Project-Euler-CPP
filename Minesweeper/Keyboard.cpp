#include "Keyboard.hpp"
#include "SafeCGEvent.hpp"

#include "../Wait.hpp"

#include <optional>
#include <thread>

template<class T>
using atomic_shared = std::shared_ptr<T>;

static std::atomic<std::thread::id> lastRepeatedHeldKeyThreadId{};

static std::atomic<int> shiftCount{0};
static std::atomic<int> controlCount{0};
static std::atomic<int> optionCount{0};
static std::atomic<int> appleCount{0};
static std::atomic<int> capsLockCount{0};
static std::atomic<int> functionCount{0};
static std::atomic<int> helpCount{0};

static std::atomic<int>* keyToModifierCount(Key key) {
    // https://developer.apple.com/documentation/coregraphics/cgeventflags
    switch (key) {
      case Key::LEFT_SHIFT:
      case Key::RIGHT_SHIFT:
	return &shiftCount;
      case Key::LEFT_CONTROL:
      case Key::RIGHT_CONTROL:
	return &controlCount;
      case Key::LEFT_OPTION:
      case Key::RIGHT_OPTION:
	return &optionCount;
      case Key::LEFT_APPLE:
      case Key::RIGHT_APPLE:
	return &appleCount;
      case Key::CAPS_LOCK:
	return &capsLockCount;
      case Key::FUNCTION:
	return &functionCount;
      case Key::HELP:
	return &helpCount;
      default:
	return nullptr;
    }
}
static void triggerKeyEvent(Key key, bool down) {
    SafeCGEvent event = CGEventCreateKeyboardEvent(nullptr, static_cast<KeyInt>(key), down);
    event.trigger();
}
static void triggerKeyDownEvent(Key key) {
    if (auto modifierCount = keyToModifierCount(key)) {
	++*modifierCount;
    }
    triggerKeyEvent(key, true);
}
static void triggerKeyUpEvent(Key key) {
    if (auto modifierCount = keyToModifierCount(key)) {
	--*modifierCount;
    }
    triggerKeyEvent(key, false);
}

static void holdKeyThread(Key key,
			  atomic_shared<std::atomic<bool>> keyStillHeld,
			  NormalDuration const& delayBeforeFirstRepeat,
			  NormalDuration const& timeBetweenRepeats) {
    auto thisThreadId = std::this_thread::get_id();
    lastRepeatedHeldKeyThreadId = thisThreadId;
    if (!delayBeforeFirstRepeat.isForever()) {
	waitFor(delayBeforeFirstRepeat);
	while (*keyStillHeld && thisThreadId == lastRepeatedHeldKeyThreadId) {
	    triggerKeyDownEvent(key);
	    waitFor(timeBetweenRepeats);
	}
    }
    lastRepeatedHeldKeyThreadId.compare_exchange_strong(thisThreadId, std::thread::id());
}

class RepeatedHeldKey : public VirtualDestructor {
  public:
    RepeatedHeldKey(Key key,
		    NormalDuration const& delayBeforeFirstRepeat,
		    NormalDuration const& timeBetween,
		    NormalDuration const& timeAfterRelease)
	: fKey(key)
	, fTimeAfterRelease(timeAfterRelease)
	, fHeld(std::make_shared<std::atomic<bool>>(true)) {
	std::thread repeatingKeyThread(holdKeyThread, key, std::atomic_load(&fHeld), delayBeforeFirstRepeat, timeBetween);
	repeatingKeyThread.detach();
    }
    ~RepeatedHeldKey() override;
    
  private:
    Key fKey;
    NormalDuration fTimeAfterRelease;
    atomic_shared<std::atomic<bool>> fHeld;
};
RepeatedHeldKey::~RepeatedHeldKey() {
    *fHeld = false;
    triggerKeyUpEvent(fKey);
    waitFor(fTimeAfterRelease);
}

class NonRepeatedHeldKey : public VirtualDestructor {
  public:
    NonRepeatedHeldKey(Key key, NormalDuration const& timeAfterRelease)
	: fKey(key)
	, fTimeAfterRelease(timeAfterRelease) {
    }
    ~NonRepeatedHeldKey() override;
	
  private:
    Key fKey;
    NormalDuration fTimeAfterRelease;
};
NonRepeatedHeldKey::~NonRepeatedHeldKey() {
    triggerKeyUpEvent(fKey);
    waitFor(fTimeAfterRelease);
}

struct ShiftAndKey {
    bool requiresShift;
    Key key;
};
static std::optional<ShiftAndKey> charToKey(char c) {
    switch (c) {
#define CASE(KEY, LOWER, UPPER)			\
	case LOWER:				\
	  return ShiftAndKey{false, Key::KEY};	\
      case UPPER:				\
	return ShiftAndKey{true, Key::KEY}

	CASE(A, 'a', 'A');
	CASE(B, 'b', 'B');
	CASE(C, 'c', 'C');
	CASE(D, 'd', 'D');
	CASE(E, 'e', 'E');
	CASE(F, 'f', 'F');
	CASE(G, 'g', 'G');
	CASE(H, 'h', 'H');
	CASE(I, 'i', 'I');
	CASE(J, 'j', 'J');
	CASE(K, 'k', 'K');
	CASE(L, 'l', 'L');
	CASE(M, 'm', 'M');
	CASE(N, 'n', 'N');
	CASE(O, 'o', 'O');
	CASE(P, 'p', 'P');
	CASE(Q, 'q', 'Q');
	CASE(R, 'r', 'R');
	CASE(S, 's', 'S');
	CASE(T, 't', 'T');
	CASE(U, 'u', 'U');
	CASE(V, 'v', 'V');
	CASE(W, 'w', 'W');
	CASE(X, 'x', 'X');
	CASE(Y, 'y', 'Y');
	CASE(Z, 'z', 'Z');
	CASE(ONE, '1', '!');
	CASE(TWO, '2', '@');
	CASE(THREE, '3', '#');
	CASE(FOUR, '4', '$');
	CASE(FIVE, '5', '%');
	CASE(SIX, '6', '^');
	CASE(SEVEN, '7', '&');
	CASE(EIGHT, '8', '*');
	CASE(NINE, '9', '(');
	CASE(ZERO, '0', ')');
      case '\t':
	return ShiftAndKey{false, Key::TAB};
	CASE(GRAVE, '`', '~');
	CASE(MINUS, '-', '_');
	CASE(EQUAL, '=', '+');
	CASE(LEFT_BRACKET, '[', '{');
	CASE(RIGHT_BRACKET, ']', '}');
	CASE(BACKSLASH, '\\', '|');
	CASE(SEMICOLON, ';', ':');
	CASE(QUOTE, '\'', '"');
      case '\n':
      case '\r':
	return ShiftAndKey{false, Key::RETURN};
	CASE(COMMA, ',', '<');
	CASE(PERIOD, '.', '>');
	CASE(SLASH, '/', '?');
      case static_cast<char>(127):
	return ShiftAndKey{false, Key::DELETE};
      case ' ':
	return ShiftAndKey{false, Key::SPACE};
#undef CASE
      default:
	return std::nullopt;
    }
}

CGEventFlags Keyboard::currentModifierFlags() {
    CGEventFlags result{0};
    if (shiftCount) {
	result |= kCGEventFlagMaskShift;
    }
    if (controlCount) {
	result |= kCGEventFlagMaskControl;
    }
    if (optionCount) {
	result |= kCGEventFlagMaskAlternate;
    }
    if (appleCount) {
	result |= kCGEventFlagMaskCommand;
    }
    if (capsLockCount) {
	result |= kCGEventFlagMaskAlphaShift;
    }
    if (functionCount) {
	result |= kCGEventFlagMaskSecondaryFn;
    }
    if (helpCount) {
	result |= kCGEventFlagMaskHelp;
    }
    return result;
}

void Keyboard::pressKey(Key key,
			NormalDuration const& timeDown,
			NormalDuration const& timeAfter) {
    auto heldKey = holdKey(key, timeDown, forever, forever, timeAfter);
}

[[nodiscard]] Keyboard::KeyRelease Keyboard::holdKey(Key key,
						     NormalDuration const& delayAfterDownThisThread,
						     NormalDuration const& delayBeforeFirstRepeatOtherThread,
						     NormalDuration const& timeBetweenRepeatsOtherThread,
						     NormalDuration const& timeAfterRelease) {
    if (!isModifier(key)) {
	lastRepeatedHeldKeyThreadId = std::thread::id{};
    }
    triggerKeyDownEvent(key);
    Keyboard::KeyRelease result;
    if (isModifier(key)) {
	result = std::make_unique<NonRepeatedHeldKey>(key, timeAfterRelease);
    } else {
	result = std::make_unique<RepeatedHeldKey>(key, delayBeforeFirstRepeatOtherThread, timeBetweenRepeatsOtherThread, timeAfterRelease);
    }
    // wait after constructing RepeatedHeldKey to not delay other thread
    waitFor(delayAfterDownThisThread);
    return result;
}

[[nodiscard]] bool Keyboard::isModifier(Key key) {
    return keyToModifierCount(key) != nullptr;
}

void Keyboard::typeText(std::string const& text,
			NormalDuration const& timeDownPerKey,
			NormalDuration const& timeAfterPerKey) {
    KeyRelease holdingShift = nullptr;
    for (char c : text) {
	if (auto shiftAndKey = charToKey(c)) {
	    auto [requiresShift, key] = *shiftAndKey;
	    if (requiresShift && !holdingShift) {
		holdingShift = holdKey(Key::SHIFT, Keyboard::DEFAULT_KEY_WAIT, forever, forever, Keyboard::DEFAULT_KEY_WAIT);
	    } else if (!requiresShift && holdingShift) {
		holdingShift = nullptr;
	    }
	    pressKey(key, timeDownPerKey, timeAfterPerKey);
	}
    }
}
