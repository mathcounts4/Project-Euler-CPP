// g++ -std=c++11 testDynamic.cpp -o testDynamic.out

#include <iostream>
#include "DynamicMatrix.hpp"

using namespace std;

int main()
{
    cout << "Declaring a<int>(3 by 4, init to 1)" << endl;
    DynamicMatrix<int> a(3,4,1);
    cout << endl;

    cout << "Printing a" << endl;
    a.print();
    cout << endl;

    cout << "Setting a[2][1] = 5, printing a" << endl;
    a[2][1] = 5;
    a.print();
    cout << endl;

    cout << "Declaring b<int> = a" << endl;
    DynamicMatrix<int> b = a;
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

    cout << "Setting b = rvalue Matrix<int> (1 by 4, init to 13)" << endl;
    b = DynamicMatrix<int>(1,4,13);
    cout << endl;

    cout << "Printing b" << endl;
    b.print();
    cout << endl;

    cout << "Declaring c<double> = rvalue Matrix<int> (4 by 6, init to 1)" << endl;
    DynamicMatrix<double> c = DynamicMatrix<int>(4,6,1);
    cout << endl;

    cout << "Printing c" << endl;
    c.print();
    cout << endl;

    cout << "Declaring d<double>(rvalue Matrix<int> (3 by 3, init to 17))" << endl;
    DynamicMatrix<double> d(DynamicMatrix<int>(3,3,17));
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

    cout << "Declaring e<double> = a" << endl;
    DynamicMatrix<double> e = a;
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

    cout << "Declaring f<int>(2 by 2, init to 1)" << endl;
    DynamicMatrix<int> f(2,2,1);
    cout << endl;

    cout << "Printing f" << endl;
    f.print();
    cout << endl;

    cout << "Declaring g<int> = f^4" << endl;
    DynamicMatrix<int> g = f^4;
    cout << endl;

    cout << "Printing g" << endl;
    g.print();
    cout << endl;
    
    return 0;
}
