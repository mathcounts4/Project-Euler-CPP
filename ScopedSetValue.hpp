#pragma once

template<class T>
class ScopedSetValue {
  private:
    T& variable;
    T originalValue;
  public:
    ScopedSetValue(T& var, T const& newValue)
	: variable(var)
	, originalValue(var) {
	variable = newValue;
    }
    ScopedSetValue(ScopedSetValue&& other)
	: variable(other.variable)
	, originalValue(static_cast<T&&>(other.originalValue)) {
	other.originalValue = other.variable;
    }
    ~ScopedSetValue() {
	try {
	    variable = originalValue;
	} catch (...) {
	    // don't throw from destructor
	}
    }
};
template<class T, class U> ScopedSetValue(T&, U&&) -> ScopedSetValue<T>;
