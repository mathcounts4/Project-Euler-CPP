#include "../Class.hpp"
#include "../PrimeUtils.hpp"
#include "../TypeUtils.hpp"

#include <iostream>

SI main(Argc const argc, Argv const argv) {
    B ranOk = true;
    for (SI i = 1; i < argc; ++i) {
	auto unsignedLong = from_string<UL>(argv[i]);
	if (!unsignedLong) {
	    std::cout << "\"" << argv[i] << "\": " << unsignedLong.cause() << std::endl;
	    ranOk = false;
	} else {
	    std::cout << Prime_Factorization(*unsignedLong) << std::endl;
	}
    }
    return !ranOk;
}
