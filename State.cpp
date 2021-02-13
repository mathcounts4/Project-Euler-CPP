#include "State.hpp"

State<std::istream>::State(std::istream& is)
    : err{is.exceptions()}
    , st{is.rdstate()}
    , pos{is.tellg()}
      {}

bool State<std::istream>::apply(std::istream& is) {
    is.exceptions(std::ios_base::iostate{});
    is.clear();
    is.seekg(pos);
    bool const seek_ok = bool(is);
    is.setstate(st);
    is.exceptions(err);
    return seek_ok;
}
