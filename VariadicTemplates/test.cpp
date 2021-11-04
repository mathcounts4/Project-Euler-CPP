#include <iostream>
#include <memory>
#include <string>

using namespace std;

static int counter = 0;

string next()
{
    return to_string(counter++);
}

void print()
{
    cout << endl;
}

template<typename X, typename... Args>
void print(X&& x, Args&&... args)
{
    cout << x << " ";
    print(args...);
}

class I
{
  public:
    I(const string& name)
    {
        string nameAdd = "I(" + next() + ")";
        i_name = name + "_" + nameAdd;
        print("contructing I:",i_name);
    }
    virtual ~I()
    {
        print("deleting I:",i_name);
    }
    virtual void pureVirt() const = 0;
    string i_name;
};

class B : public I
{
  public:
    B(const string& name) : I(name)
    {
        string nameAdd = "B(" + next() + ")";
        b_name = i_name + nameAdd;
        print("constructing B:",b_name);
    }
    ~B()
    {
        print("deleting B:",b_name);
    }
    void pureVirt() const
    {
        print("hi from B:",b_name);
    }
    string b_name;
};

class A : public I
{
  public:
    A(unique_ptr<I>& I_ptr) :
            m_b1("first_member"),
            m_b2("second_member"),
            m_I_ptr(),
            I("make_a")
    {
        string nameAdd = "A(" + next() + ")";
        a_name = i_name + nameAdd;
        print("constructing A:",a_name);
        print(a_name,"taking control of",I_ptr->i_name);
        m_I_ptr.swap(I_ptr);
    }
    ~A()
    {
        print("deleting A:",a_name);
    }
    void pureVirt() const
    {
        print("hi from A:",a_name);
    }
    B m_b1;
    unique_ptr<I> m_I_ptr;
    B m_b2;
    string a_name;
};

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(args...));
}

template<typename AbstractBase, typename Derived, typename... Args>
unique_ptr<AbstractBase> my_make_unique(Args&&... args)
{
    return unique_ptr<AbstractBase>(new Derived(args...));
}

int main()
{
    unique_ptr<A> A_ptr = make_unique<A>(my_make_unique<I,B>("make_b"));

    print();

    print("A_ptr says:");
    A_ptr->pureVirt();

    print("A_ptr's I_ptr says:");
    A_ptr->m_I_ptr->pureVirt();

    print();

    print(1,"+",2,"=",3,"Variadic","templates","are","the","coolest.");

    print("Here","we","learn","about","templates","and","the","order",
          "of","initialization","and","deletion.");

    print();
    print("Initialization:");
    print("1.","subclasses");
    print("2.","member variables (in the order declared)");
    print("3.","constructor");

    print();
    print("Deletion:");
    print("1.","destructor");
    print("2.","member variables (in reverse order declared)");
    print("3.","subclasses");

    print();

    return 0;
}

