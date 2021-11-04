#include "ExitUtils.hpp"
#include "IOUtils.hpp"

int get_ios(termios& ios, bool throw_on_failure) {
    char const * const failure = "Failed to get iosettings";
    if (tcgetattr(0,&ios)) {
	if (throw_on_failure)
	    throw_exception<std::ios_base::failure>(failure);
	std::cout << failure << std::endl;
	return 1;
    }
    return 0;
}

int set_ios(termios const& ios, bool throw_on_failure) {
    char const * const failure = "Failed to set iosettings";
    if (tcsetattr(0,TCSANOW,&ios)) {
	if (throw_on_failure)
	    throw_exception<std::ios_base::failure>(failure);
	std::cout << failure << std::endl;
	return 1;
    }
    return 0;
}

int disable_echo(bool throw_on_failure) {
    termios ios;
    if (get_ios(ios,throw_on_failure))
	return 1;
    ios.c_lflag &= ~static_cast<UL>(ECHO);
    if (set_ios(ios,throw_on_failure))
	return 1;
    return 0;
}

int enable_echo(bool throw_on_failure) {
    termios ios;
    if (get_ios(ios,throw_on_failure))
	return 1;
    ios.c_lflag |= static_cast<UL>(ECHO);
    if (set_ios(ios,throw_on_failure))
	return 1;
    return 0;
}

DisableEcho::DisableEcho(bool throw_on_failure)
    : ios{}
    , had_echo(false) {
	if (get_ios(ios,throw_on_failure))
	    return;
	had_echo = ios.c_lflag & static_cast<UL>(ECHO);
	if (had_echo) {
	    ios.c_lflag &= ~static_cast<UL>(ECHO);
	    set_ios(ios,throw_on_failure);
	}
      }
DisableEcho::~DisableEcho() {
    if (had_echo) {
	ios.c_lflag |= static_cast<UL>(ECHO);
	// never throw in a destructor
	set_ios(ios,false);
    }   
}
