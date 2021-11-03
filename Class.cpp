#include "Class.hpp"

Optional<void> consume_opt(std::istream& is, char const c) {
    Resetter resetter{is};
    std::ws(is);

    auto failure = [c](std::string const& cause) -> Failure
    {
        return std::string("Expected to consume '") + c + "' but " + cause;
    };
    
    if (is.eof())
        return failure("stream hit eof");
    if (is.bad())
	return failure("stream is in bad state");

    char const _c = static_cast<char>(is.get());
    if (_c != c)
	return failure(std::string("encountered '") + _c + "'");

    resetter.ignore();
    return {};
}

bool consume(std::istream& is, char const c) {
    Resetter resetter{is};
    std::ws(is);
    if (!is)
	return false;
    if (is.peek() != c)
	return false;
    is.get();
    resetter.ignore();
    return true;
}

Optional<void> consume_opt(std::istream& is, std::string const& str) {
    Resetter resetter{is};
    std::ws(is);

    auto failure = [str](std::string const& cause) -> Failure
	{
	    return "Expected to consume \"" + str + "\" but " + cause;
	};
    
    for (char const c : str) {
	if (is.eof())
	    return failure("stream hit eof");
	if (is.bad())
	    return failure("stream is in bad state");

	char const _c = static_cast<char>(is.get());
	if (_c != c)
	    return failure(std::string("encountered '") + _c + "' when looking for '" + c + '\'');
    }
    resetter.ignore();
    return {};
}
bool consume(std::istream& is, std::string const& str) {
    Resetter resetter{is};
    std::ws(is);
    for (char const c : str) {
	if (!is)
	    return false;
	if (is.peek() != c)
	    return false;
	is.get();
    }
    resetter.ignore();
    return true;
}

template class Resetter<std::istream>;

