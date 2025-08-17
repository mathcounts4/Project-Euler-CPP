#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <type_traits>

/*
This file determines strategy and probability of success for specific scenarios in the game "Connections" by the New York Times.
Connections involves correctly determining the groupings of 4 groups of 4 words, from the unordered 16 words.
You submit guesses of groups of 4, and are told if the guess is correct, contains 3 words in the same group, or neither.
Here, we investigate the optimal strategy/strategies when:
1. You have already solved 2 of the groups, so you have 8 words remaining
2. You have no idea which remaining words should be grouped together
3. You have 1, 2, 3, or 4 guesses remaining
*/

template<auto VecCreator>
constexpr auto vecToArray() {
    auto vec = VecCreator();
    std::array<std::remove_cvref_t<decltype(vec[0])>, VecCreator().size()> arr;
    std::copy(vec.begin(), vec.end(), arr.begin());
    return arr;
}

template<class T>
static constexpr std::uint8_t numBitsSet(T t) {
    std::uint8_t bits = 0;
    while (t) {
	t = t & (t - 1);
	++bits;
    }
    return bits;
}

static constexpr std::uint8_t opposite(std::uint8_t x) {
    return static_cast<std::uint8_t>(255) - x;
}

static constexpr auto determineGroups() {
    std::vector<std::uint8_t> groups;
    for (std::uint16_t x16 = 0; x16 <= std::numeric_limits<uint8_t>::max(); ++x16) {
	std::uint8_t x = static_cast<std::uint8_t>(x16);
	if (numBitsSet(x) != 4) {
	    continue;
	}
	if (x % 2 == 0) {
	    // Always include this bit in the group:
	    // Since we're splitting 8 things into 2 groups of 4, and guessing either group is correct,
	    //   guessing a group without this bit is equivalent to guessing the other 4 things, with this bit.
	    continue;
	}
	groups.push_back(x);
    }
    return groups;
}
constexpr auto groups = vecToArray<determineGroups>();
using GuessSet = std::bitset<groups.size()>;
constexpr auto guessMultFactor = []() -> uint8_t {
    std::array<bool, 256> allGroups{};
    for (auto group : groups) {
	allGroups[group] = true;
    }
    for (auto group : groups) {
	if (!allGroups[opposite(group)]) {
	    return 1;
	}
    }
    return 2;
}();

enum class Outcome { Correct, OneAway, Default };
static char const* outcomeToString(Outcome outcome) {
    switch (outcome) {
      case Outcome::Correct:
	return "Correct";
      case Outcome::OneAway:
	return "One away";
      case Outcome::Default:
	return "Not correct or one away";
    }
}

// GuessChain = a sequence of guesses and outcomes
// For example, the following sequence of 3 guesses + their outcomes:
// 1. 4 random words -> default (not correct or one away)
// 2. 2 random words in the first guess and 2 random words not in the first guess -> one away
// 3. 2 random words in the first and second guesses, 1 random word in the second guess but not the first, and 1 random word in neither -> one away
template<std::size_t numGuesses>
struct GuessChain {
    std::uint8_t fGuess;
    Outcome fOutcome;
    GuessChain<numGuesses - 1> const& fPreviousGuesses;
};
template<>
struct GuessChain<0> {};

// Guess<2> x where:
// x.fCounts___InPreviousGuess.fCounts___InPreviousGuess.fCount = 2
// x.fCounts___InPreviousGuess.fCountsNotInPreviousGuess.fCount = 0
// x.fCountsNotInPreviousGuess.fCounts___InPreviousGuess.fCount = 1
// x.fCountsNotInPreviousGuess.fCountsNotInPreviousGuess.fCount = 1
// means that x contains:
// * 2 values that were in both of the prior guesses
// * 1 value that was in the first guess but not the second
// * 1 value that was in neither the first guess nor the second
template<std::size_t numPreviousGuesses>
struct Guess {
    Guess<numPreviousGuesses - 1> fCounts___InPreviousGuess;
    Guess<numPreviousGuesses - 1> fCountsNotInPreviousGuess;

    Guess(std::uint8_t guess, GuessChain<numPreviousGuesses> const& previousGuesses)
	: fCounts___InPreviousGuess(guess & previousGuesses.fGuess, previousGuesses.fPreviousGuesses)
	, fCountsNotInPreviousGuess(guess & opposite(previousGuesses.fGuess), previousGuesses.fPreviousGuesses) {}

    friend auto operator<=>(Guess const&, Guess const&) = default;

    void display(std::string const& suffix) const {
	fCounts___InPreviousGuess.display(" in guess " + std::to_string(numPreviousGuesses) + suffix);
	fCountsNotInPreviousGuess.display(" NOT in guess " + std::to_string(numPreviousGuesses) + suffix);
    }
};
template<>
struct Guess<0> {
    std::uint8_t fCount;

    Guess(std::uint8_t guess, GuessChain<0> const& /* previousGuesses */)
	: fCount(numBitsSet(guess)) {}

    friend auto operator<=>(Guess const&, Guess const&) = default;

    void display(std::string const& suffix) const {
	if (fCount) {
	    if (fCount == 1) {
		std::cout << "1 word" << suffix;
	    } else {
		std::cout << static_cast<std::size_t>(fCount) << " words" << suffix;
	    }
	}
    }
};

static Outcome computeOutcome(std::uint8_t guess, std::uint8_t actual) {
    switch (numBitsSet(guess & actual)) {
      case 4:
      case 0:
	return Outcome::Correct;
      case 3:
      case 1:
	return Outcome::OneAway;
      default:
	return Outcome::Default;
    }
}

static void dispIndent(std::size_t indent) {
    std::cout << std::string(indent, ' ');
}

struct DisplayBlock {
    std::size_t& fIndent;
    DisplayBlock(std::size_t& indent)
	: fIndent(indent) {
	std::cout << "{" << std::endl;
	++fIndent;
    }
    ~DisplayBlock() {
	--fIndent;
	dispIndent(fIndent);
	std::cout << "}" << std::endl;
    }
};
[[nodiscard]] static DisplayBlock displayBlock(std::size_t& indent) {
    return {indent};
}

// Strategy: informs which guesses can be used, and what strategy to use after each guess and its result
// If an outcome is "Correct", the corresponding strategy has no guess options, but success probability = 1.
template<std::size_t numGuessesLeft, std::size_t numGuessesUsed>
struct Strategy {
    double fSuccessProbability{0};
    bool fEveryGuessThatCouldBeAnAnswerIsAnOptimalStrategy{false};
    bool fEveryOptimalStrategyIsPotentiallyCorrectGuess{false};
    std::map<Guess<numGuessesUsed>, std::map<Outcome, std::pair<std::uint8_t /* count */, Strategy<numGuessesLeft - 1, numGuessesUsed + 1>>>> fGuessOptions;

    void display(std::size_t indent) const {
	std::cout << "success = " << fSuccessProbability << " ";
	std::cout << "potentially-correct guess ";
	if (fEveryGuessThatCouldBeAnAnswerIsAnOptimalStrategy) {
	    if (fEveryOptimalStrategyIsPotentiallyCorrectGuess) {
		std::cout << "<-->";
	    } else {
		std::cout << "-->";
	    }
	} else {
	    if (fEveryOptimalStrategyIsPotentiallyCorrectGuess) {
		std::cout << "<--";
	    } else {
		std::cout << "≠";
	    }
	}
	std::cout << " optimal strategy ";
	auto strategyBlock = displayBlock(indent);
	for (auto const& [guess, outcomeToCountAndStrategy] : fGuessOptions) {
	    dispIndent(indent);
	    guess.display(", ");
	    auto guessBlock = displayBlock(indent);
	    std::uint8_t totalCount = 0;
	    for (auto const& [outcome, countAndStrategy] : outcomeToCountAndStrategy) {
		auto const& [count, strategy] = countAndStrategy;
		totalCount += count;
	    }
	    for (auto const& [outcome, countAndStrategy] : outcomeToCountAndStrategy) {
		auto const& [count, strategy] = countAndStrategy;
		dispIndent(indent);
		std::cout << "(" << static_cast<std::size_t>(count / guessMultFactor) << "/" << static_cast<std::size_t>(totalCount / guessMultFactor) << ") " << outcomeToString(outcome);
		if (outcome == Outcome::Correct) {
		    std::cout << " -> win" << std::endl;
		} else {
		    std::cout << " -> ";
		    strategy.display(indent);
		}
	    }
	}
    }
};
template<std::size_t numGuessesUsed>
struct Strategy<0, numGuessesUsed> {
    double fSuccessProbability{0};

    void display(std::size_t /* indent */) const {
	std::cout << "lose" << std::endl;
    }
};

template<std::size_t numGuessesLeft, std::size_t numGuessesUsed, bool pickOneOptimalStrategy>
static Strategy<numGuessesLeft, numGuessesUsed> solveRecursive(GuessChain<numGuessesUsed> const& prevGuesses,
							       GuessSet const& possibleAnswers) {
    if constexpr (numGuessesLeft == 0) {
	return {};
    } else {
	// Try each guess.
	//   Optimize by skipping guesses that are identical to other ones (but including these in the total weight).
	//   For example, if we guessed [1 2 3 4] and got "one away", [1 2 3 5] is equivalent to [1 2 3 6].
	//   For each possible outcome, determine the optimal strategy, and its probability of success.
	//   For this guess, determine the probability of each outcome.
	//   For this guess, calculate the overall probability of success.
	// Looking at all possible guesses, find the one(s) with the highest probability of success, and remove the others.
	Strategy<numGuessesLeft, numGuessesUsed> result;
	std::vector<std::pair<Guess<numGuessesUsed>, double>> successProbability;

	// Try guesses
	for (std::uint8_t guess : groups) {
	    Guess<numGuessesUsed> newGuess(guess, prevGuesses);
	    if (result.fGuessOptions.contains(newGuess)) {
		continue;
	    }
	    std::map<Outcome, std::pair<std::uint8_t /* count */, GuessSet>> validActuals;
	    std::uint8_t numValidActuals = 0;
	    for (std::size_t actualIndex = 0; actualIndex < groups.size(); ++actualIndex) {
		std::uint8_t actual = groups[actualIndex];
		if (possibleAnswers[actualIndex]) {
		    Outcome outcome = computeOutcome(guess, actual);
		    ++numValidActuals;
		    ++validActuals[outcome].first;
		    validActuals[outcome].second[actualIndex] = true;
		}
	    }
	    double weightedSum = 0;
	    for (auto const& [outcome, countAndActuals] : validActuals) {
		auto [count, actualIndices] = countAndActuals;
		Strategy<numGuessesLeft - 1, numGuessesUsed + 1> strategyForOutcome;
		if (outcome == Outcome::Correct) {
		    strategyForOutcome.fSuccessProbability = 1;
		} else {
		    GuessChain<numGuessesUsed + 1> newGuesses{guess, outcome, prevGuesses};
		    strategyForOutcome = solveRecursive<numGuessesLeft - 1, numGuessesUsed + 1, pickOneOptimalStrategy>(newGuesses, actualIndices);
		}
		weightedSum += strategyForOutcome.fSuccessProbability * count;
		result.fGuessOptions[newGuess][outcome] = {count, std::move(strategyForOutcome)};
	    }
	    auto probabilityForGuess = weightedSum / numValidActuals;
	    successProbability.push_back({newGuess, probabilityForGuess});
	}

	// Determine best guess(es)
	double bestProbability = 0;
	for (auto const& [newGuess, probabilityForGuess] : successProbability) {
	    bestProbability = std::max(bestProbability, probabilityForGuess);
	}
	result.fSuccessProbability = bestProbability;
	// Filter to optimal guesses
	for (auto const& [newGuess, probabilityForGuess] : successProbability) {
	    // Arbitrary small fudge factor for floating-point rounding inconsistencies
	    if (probabilityForGuess + 1e-7 < bestProbability) {
		result.fGuessOptions.erase(newGuess);
	    }
	}
	// Check if all guesses of potentially-correct guesses are an optimal strategy
	bool everyGuessThatCouldBeAnAnswerIsAnOptimalStrategy = true;
	for (std::size_t testIndex = 0; testIndex < groups.size(); ++testIndex) {
	    std::uint8_t test = groups[testIndex];
	    if (possibleAnswers[testIndex]) {
		Guess<numGuessesUsed> testGuess(test, prevGuesses);
		if (!result.fGuessOptions.contains(testGuess)) {
		    everyGuessThatCouldBeAnAnswerIsAnOptimalStrategy = false;
		    break;
		}
	    }
	}
	result.fEveryGuessThatCouldBeAnAnswerIsAnOptimalStrategy = everyGuessThatCouldBeAnAnswerIsAnOptimalStrategy;
	// Check if all optimal strategies are potentially-correct guesses
	bool everyOptimalStrategyIsPotentiallyCorrectGuess = true;
	std::set<Guess<numGuessesUsed>> correctGuesses;
	for (std::size_t testIndex = 0; testIndex < groups.size(); ++testIndex) {
	    std::uint8_t test = groups[testIndex];
	    if (possibleAnswers[testIndex]) {
		Guess<numGuessesUsed> testGuess(test, prevGuesses);
		correctGuesses.insert(testGuess);
	    }
	}
	for (auto const& [guess, outcomeStrategy] : result.fGuessOptions) {
	    if (!correctGuesses.contains(guess)) {
		everyOptimalStrategyIsPotentiallyCorrectGuess = false;
		break;
	    }
	}
	result.fEveryOptimalStrategyIsPotentiallyCorrectGuess = everyOptimalStrategyIsPotentiallyCorrectGuess;
	// Reduce strategy to one choice
	if constexpr (pickOneOptimalStrategy) {
	    result.fGuessOptions = {*result.fGuessOptions.rbegin()};
	}
	return result;
    }
}

template<std::size_t numGuesses, bool pickOneOptimalStrategy>
static Strategy<numGuesses, 0> solve() {
    GuessSet allValid;
    allValid.set(); // set all 
    return solveRecursive<numGuesses, 0, pickOneOptimalStrategy>({}, allValid);
}

int main() {
    // Given a series of guesses and outcomes, determine:
    // 1. the best strategy: the next set of allowed guesses that maximize probability of success
    // 2. the probability of success of the best strategy
    std::cout << "unique strategy for 1 guess:" << std::endl;
    solve<1, true>().display(0);
    std::cout << std::endl;
    
    std::cout << "unique strategy for 2 guesses:" << std::endl;
    solve<2, true>().display(0);
    std::cout << std::endl;
    
    std::cout << "all strategies for 2 guesses:" << std::endl;
    solve<2, false>().display(0);
    std::cout << std::endl;
    
    std::cout << "unique strategy for 3 guesses:" << std::endl;
    solve<3, true>().display(0);
    std::cout << std::endl;
    
    std::cout << "unique strategy for 4 guesses:" << std::endl;
    solve<4, true>().display(0);
    std::cout << std::endl;
    
    return 0;
}

/*
  We see that, for small numbers of guesses ≤ 4, most of the time, the optimal strategies are exactly the guesses of groups that could be correct.
  However, there is exactly 1 exception:
  With 4 guesses, after 2 guesses with outcomes:
  1. 1234 (4 random) -> 2 away
  2. 1256 (2 in first guess, 2 not in first guess) -> 2 away
  Then the possible groups that could be correct (including 1) are these 9:
  1278
  1357
  1358
  1367
  1368
  1457
  1458
  1467
  1468
  Note the final 8 are equivalent: choose 3 or 4, choose 5 or 6, choose 7 or 8.
  Any of these 8 possible-correct groups is an optimal strategy (success = 1/3).
  But the first of these is not an optimal strategy (success = 2/9).
  1278 -> 1/9 correct, 8/9 -> 2 away -> *1/8. total = 1/9 + 8/9*1/8 = 2/9
  1357 -> 1/9 correct, 4/9 -> 1 away -> *1/4, 4/9 -> 2 away -> *1/4. total = 1/9 + 4/9*1/4 + 4/9*1/4 = 1/3
 */
