#include "Blabber.hpp"
#include "Class.hpp"

#define BLABBER_PRINT std::cout << __PRETTY_FUNCTION__ << std::endl



void Blabber::discard() {
    x = {};
}
bool Blabber::ok() const {
    return x.size() == 1 && x[0] == 12345;
}
void Blabber::check() const {
    if (!ok())
	throw_exception<std::runtime_error>("Discarded Blabber was used");
}
    
Blabber::Blabber()
    : x{12345} {
    BLABBER_PRINT;
}
Blabber::Blabber(Blabber & o)
    : x(o.x) {
    BLABBER_PRINT;
}
Blabber::Blabber(Blabber const & o)
    : x(o.x) {
    BLABBER_PRINT;
}
Blabber::Blabber(Blabber && o)
    : x(o.x) {
    o.discard();
    BLABBER_PRINT;
}
Blabber::Blabber(Blabber const && o)
    : x(o.x) {
    BLABBER_PRINT;
}
Blabber::~Blabber() {
    discard();
}
Blabber& Blabber::operator=(Blabber & o) {
    BLABBER_PRINT;
    x=o.x;
    return *this;
}
Blabber& Blabber::operator=(Blabber const & o) {
    BLABBER_PRINT;
    x=o.x;
    return *this;
}
Blabber& Blabber::operator=(Blabber && o) {
    BLABBER_PRINT;
    x=o.x;
    o.discard();
    return *this;
}
Blabber& Blabber::operator=(Blabber const && o) {
    BLABBER_PRINT;
    x=o.x;
    return *this;
}



#undef BLABBER_PRINT



template<> std::ostream& Class<Blabber>::print(std::ostream& os, Blabber const & b) {
    return os << (b.ok() ? "Blabber_OK" : "Blabber_CORRUPTED");
}

template<> Optional<Blabber> Class<Blabber>::parse(std::istream&) {
    return {};
}

template<> std::string Class<Blabber>::name() {
    return "Blabber";
}

template<> std::string Class<Blabber>::format() {
    return "<no characters>";
}
