#include <iostream>

#include <dlfcn.h>

// g++ -o runme main.cpp

double lib(double x);

int main(int argc, char** argv)
{
    void* lib_handle = dlopen("libbuild.dylib", RTLD_LOCAL|RTLD_LAZY);
    if (!lib_handle) {
	printf("%s\n", dlerror());
	exit(EXIT_FAILURE);
    }
 
    // Print data entered and call libRatings.A:addRating().
    int (*fcn)(void) = (int(*)(void)) dlsym(lib_handle, "entry");
    if (!fcn) {       // addRating is guaranteed to exist in libRatings.A.dylib
	printf("[%s] Unable to get symbol: %s\n", __FILE__, dlerror());
	exit(EXIT_FAILURE);
    }

    std::cout << fcn() << std::endl;
}
