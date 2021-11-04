// g++ -shared -L. -llib -o libbuild.dylib buildLib.cpp

double lib(double x);

extern "C"
{
    int entry()
    {
	return lib(3);
    }
}
