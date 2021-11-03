#pragma once

#include <istream>

// State<T>::State(T const&) should store any necessary state in members
// State<T>.apply(T&) should apply the stored state to the input and return if it succeeded
template<class T> class State;

template<> class State<std::istream> {
  private:
    std::ios_base::iostate const err;
    std::ios_base::iostate const st;
    std::streampos const pos;
  public:
    State(std::istream& is); // cannot be const since tellg isn't const
    bool apply(std::istream& is);
};
