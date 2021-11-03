#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>

#include "Class.hpp"
#include "ExitUtils.hpp"

struct Blabber {
  private:
    std::vector<int> x;
  public:
    void discard();
    bool ok() const;
    void check() const; // throws if not OK
    
    Blabber();
    Blabber(Blabber      &  o);
    Blabber(Blabber const&  o);
    Blabber(Blabber      && o);
    Blabber(Blabber const&& o);
    ~Blabber();
    Blabber& operator=(Blabber      &  o);
    Blabber& operator=(Blabber const&  o);
    Blabber& operator=(Blabber      && o);
    Blabber& operator=(Blabber const&& o);
};

template<> std::ostream& Class<Blabber>::print(std::ostream& os, Blabber const& b);
template<> Optional<Blabber> Class<Blabber>::parse(std::istream&);
template<> std::string Class<Blabber>::name();
template<> std::string Class<Blabber>::format();
