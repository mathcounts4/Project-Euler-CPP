// g++ -shared -o liblib.dylib lib.cpp
// g++ -shared -DBAD -o liblib.dylib lib.cpp

double lib(double x);

#ifndef WATUP
#define WATUP
double lib(double x)
{
    return x+1;
}
#endif
