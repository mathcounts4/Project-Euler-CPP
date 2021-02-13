#include "BetterRegex.hpp"

namespace regex {
    
    Optional optional;

    void LiteralRegex::print(std::ostream& os) const {
	static constexpr char needEscape[] = R"(^$\.*+?()[]{}|)";
	for (char c : fLiteral) {
	    for (char x : needEscape)
		if (c == x) {
		    os << '\\';
		    break;
		}
	    os << c;
	}
    }
    
} /* namespace regex */

