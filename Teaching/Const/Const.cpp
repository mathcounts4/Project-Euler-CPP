#include <cstdlib>
#include <iostream>

struct Complex
{
    int r,i;
    Complex(int r, int i) : r(r), i(i) {}

    int norm() const { return r*r + i*i; }
    Complex conj() const { return Complex(r,-i); }
    
    Complex operator-() { return {-r,-i}; }
    
    Complex& operator+=(Complex const & o) {
	r += o.r; i += o.i; return *this; }
    Complex operator+(Complex const & o) {
	return Complex(*this) += o; }
    
    Complex& operator-=(Complex const & o) {
	r -= o.r; i -= o.i; return *this; }
    Complex operator-(Complex const & o) {
	return Complex(*this) -= o; }
    
    Complex& operator*=(Complex const & o) {
	return *this = {r*o.r-i*o.i,r*o.i+i*o.r}; }
    Complex operator*(Complex const & o) {
	return Complex(*this) *= o; }
    
    Complex& operator/=(Complex const & o) {
	(*this) *= o.conj();
	long norm = o.norm();
	r /= norm;
	i /= norm;
	return *this;
    }
    Complex operator/(Complex const & o) {
	return Complex(*this) /= o; }

    friend std::ostream& operator<<(std::ostream& o, Complex const & c)
	{
	    bool started = false;
	    if (c.r)
	    {
		o << c.r;
		started = true;
	    }
	    if (c.i)
	    {
		if (c.i>0 && started)
		    o << "+";
		if (c.i<0)
		    o << "-";
		if (std::abs(c.i) != 1)
		    o << std::abs(c.i);
		o << "i";
		started = true;
	    }
	    if (!started)
		o << "0";
	    return o;
	}
};

using namespace std;

int main()
{
    Complex A(3,5), B(1,1);
    cout << "Start: A = " << A << " and B = " << B << endl;
    A /= B;
    cout << "After A /= B, A = " << A << " and  A*B = " << A*B << endl;

    Complex C(2,2);
    cout << "Start: C = " << C << endl;
    C /= C;
    cout << "After C /= C, C = " << C << endl;
    cout << "What happened?" << endl;
}
