// X with a thread member and Y* member. Access dynamically-allocated prop of Y from second thread that stores X*.

#include <iostream>
#include <thread>

using namespace std;

struct Y
{
    volatile int * const prop;
    Y() : prop(new int(1)) {}
    ~Y() {
	delete prop;
    }
};

struct X;
void t1(X const * x);

struct X
{
    Y* y;
    thread t;
    X() : y(new Y), t(t1,this) {}
    ~X() {
	t.join();
	delete y;
    }
    void do_work_with_y() const {
        long res = 0;
	for (long i = 0; i < 1e9; ++i)
	    res += *(y->prop);
	++(*(y->prop));
	cout << "thread " << this_thread::get_id() << ": " << res << endl;
    }
};

void t1(X const * x) {
    x->do_work_with_y();
}

int main() {
    cout << "main thread " << this_thread::get_id() << endl;
    X x;
    x.do_work_with_y();
    return 0;
}
