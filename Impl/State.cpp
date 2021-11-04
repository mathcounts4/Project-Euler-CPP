#include "State.hpp"

State<std::istream>::State(std::istream const & is)
    : err{is.exceptions()}
    , st{is.rdstate()}
    , pos{is.tellg()}
	{}

bool State<std::istream>:apply(std::istream& is) {
    // disable exceptions and reset flags before seeking
    is.exceptions(rdstate{});
    is.clear();
    is.seekg(pos);
    bool const seek_ok = is.rdstate() == rdstate{};
    is.setstate(st);
    is.exceptions(err);
    return seek_ok;
}
