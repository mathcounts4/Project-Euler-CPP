// g++ -std=c++11 testStatic.cpp -o testStatic.out

#include <iostream>
#include "StaticMatrix.hpp"

using namespace std;

int main()
{
    cout << "Declaring a<int,3,4>(init to 1)" << endl;
    StaticMatrix<int,3,4> a(1);
    cout << endl;

    cout << "Printing a" << endl;
    a.print();
    cout << endl;

    cout << "Setting a[2][1] = 5, printing a" << endl;
    a[2][1] = 5;
    a.print();
    cout << endl;

    cout << "Declaring b<int,3,4> = a" << endl;
    StaticMatrix<int,3,4> b = a;
    cout << endl;

    cout << "Printing b" << endl;
    b.print();
    cout << endl;

    cout << "Setting b[1][1] = 9, printing b" << endl;
    b[1][1] = 9;
    b.print();
    cout << endl;

    cout << "Setting b = a" << endl;
    b = a;
    cout << endl;

    cout << "Printing b" << endl;
    b.print();
    cout << endl;

    cout << "Setting b = rvalue Matrix<int,3,4> (init to 13)" << endl;
    b = StaticMatrix<int,3,4>(13);
    cout << endl;

    cout << "Printing b" << endl;
    b.print();
    cout << endl;

    cout << "Declaring c<double,4,3> = rvalue Matrix<int,4,3> (init to 1)" << endl;
    StaticMatrix<double,4,3> c = StaticMatrix<int,4,3>(1);
    cout << endl;

    cout << "Printing c" << endl;
    c.print();
    cout << endl;

    cout << "Declaring d<double,3,3>(rvalue Matrix<int,3,3> (init to 17))" << endl;
    StaticMatrix<double,3,3> d(StaticMatrix<int,3,3>(17));
    cout << endl;

    cout << "Printing d" << endl;
    d.print();
    cout << endl;

    cout << "Setting d = a*c" << endl;
    d = a * c;
    cout << endl;

    cout << "Printing d" << endl;
    d.print();
    cout << endl;

    cout << "Declaring e<double,3,4> = a" << endl;
    StaticMatrix<double,3,4> e = a;
    cout << endl;

    cout << "Printing e" << endl;
    e.print();
    cout << endl;

    cout << "Setting e = a" << endl;
    e = a;
    cout << endl;

    cout << "Printing e" << endl;
    e.print();
    cout << endl;

    cout << "Declaring f<int,2,2>(init to 1)" << endl;
    StaticMatrix<int,2,2> f(1);
    cout << endl;

    cout << "Printing f" << endl;
    f.print();
    cout << endl;

    cout << "Declaring g<int,2,2> = f^5" << endl;
    StaticMatrix<int,2,2> g = f^5;
    cout << endl;

    cout << "Printing g" << endl;
    g.print();
    cout << endl;
    
    return 0;
}
