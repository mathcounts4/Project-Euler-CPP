#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "Optional.hpp"
#include "System.hpp"

Optional<Command_Result> system_call(std::string const & command) {
    std::FILE * const pipe = popen(&command[0],"r");
    if (!pipe)
	return Failure("Could not open pipe for command:\n" + command);
    constexpr SZ size = 256;
    std::array<char,size> buf;
    std::string output;
    
    while (!std::feof(pipe))
	if (nullptr != std::fgets(buf.data(),size,pipe))
	    output += buf.data();

    int const return_value = pclose(pipe);
    return Command_Result{return_value,std::move(output)};
}
