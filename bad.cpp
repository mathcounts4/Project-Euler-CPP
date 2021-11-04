class Foo {};

template<class X,class Y> struct Container_Two
{
	X x;
	Y y;
	Container_Two(X x, Y y) : x(x), y(y) {}
};

int main() {
	Container_Two<Foo,void> c3;
}
