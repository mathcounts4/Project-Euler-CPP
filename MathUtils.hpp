#pragma once
#ifndef MATH_UTILS
#define MATH_UTILS

#include <cstdlib>
#include <ostream>
#include <string>
#include <type_traits>

#include "ExitUtils.hpp"
#include "StringUtils.hpp"

// my_gcd: always returns an unsigned value
template<class T>
typename std::make_unsigned<T>::type my_gcd(T x, T y)
{
    using U = typename std::make_unsigned<T>::type;
    using S = typename std::make_signed<T>::type;

    constexpr bool is_u = std::is_unsigned<T>::value;

    U a = is_u ? x : std::abs(S(x));
    U b = is_u ? y : std::abs(S(y));
    
    if (b == 0)
	return a;
    for (U c; (c = a % b) > 0; )
    {
	a = b;
	b = c;
    }
    return b;
}

unsigned long my_pow(unsigned long b, unsigned long e)
{
    unsigned long result = 1;
    for (; e; e>>=1)
    {
	if (e&1)
	    result *= b;
	b *= b;
    }
    return result;
}



// mod_inv: computes the inverse of x mod y, with x and y coprime
// if x and y are not coprime, this halts execution
// if y == 0, this halts execution
unsigned long mod_inv(unsigned long x, unsigned long y)
{
    if (y == 0)
	my_exit("mod_inv called with mod = 0");
    unsigned long original_x = x;
    unsigned long mod = y;

    /*
      Original algorithm can be seen below in this block comment
      However, we do not need to keep track of multiples of y,
      so below this is a more efficient algorithm.
    
    // split into 2: multiples of x + multiples of y
    // start with:
    // x         = [1,0]
    // y         = [0,1]
    // x%y       = [x] - (x/y)*[y]
    // y%(x%y)   = [y] - (y/(x%y))*[x%y]
    // ...
    // 1         = [a,b]

    // a * x + b * y = 1, so x^-1 mod y = a

    // storage for 3 pairs going back in time,
    // with most recent at the front
    //                         y     x
    unsigned long prev[2][2]{{0,1},{1,0}};
    bool negative[2][2] = {{false,false},{false,false}};

    for (unsigned long z; (z = x % y) > 0; )
    {
	// div = x/y is stored in x
	x /= y;

	for (std::size_t index = 0; index < 2; ++index)
	{
	    unsigned long current;
	    bool current_negative;

	    // older - div * last
	    // prev[1] - div * prev[0]
	    // two terms in the sum, so whichever is bigger carries the sign
	    if (prev[1][index] > x * prev[0][index])
	    {
		current = prev[1][index];
		current = negative[1][index] == negative[0][index] ?
		    current - x * prev[0][index] :
		    current + x * prev[0][index];
		
		current_negative = negative[1][index];
	    }
	    else
	    {
		current = x * prev[0][index];
		current = negative[1][index] == negative[0][index] ?
		    current - prev[1][index] :
		    current + prev[1][index];

		current_negative = !negative[0][index];
	    }
	    
	    // shift previous values
	    prev[1][index] = prev[0][index];
	    negative[1][index] = negative[0][index];
	    prev[0][index] = current;
	    negative[0][index] = current_negative;
	}
	
	x = y;
	y = z;
    }
    
    if (y != 1)
	my_exit("mod_inv called with non-coprime integers " +
		std::to_string(original_x) +
		" and " +
		std::to_string(mod));
    
    return prev[0][0] ? negative[0][0] ? mod-prev[0][0] : prev[0][0] : 0;

    */

    unsigned long prev[2]{0,1};
    bool negative[2]{false,false};

    for (unsigned long z; (z = x % y) > 0; )
    {
	// div = x/y is stored in x
	x /= y;
	
	unsigned long current;
	bool current_negative;
	
	// older - div * last
	// prev[1] - div * prev[0]
	// two terms in the sum, so whichever is bigger carries the sign
	if (prev[1] > x * prev[0])
	{
	    current = prev[1];
	    current = negative[1] == negative[0] ?
		current - x * prev[0] :
		current + x * prev[0];
	    
	    current_negative = negative[1];
	}
	else
	{
	    current = x * prev[0];
	    current = negative[1] == negative[0] ?
		current - prev[1] :
		current + prev[1];
	    
	    current_negative = !negative[0];
	}
	
	// shift previous values
	prev[1] = prev[0];
	negative[1] = negative[0];
	prev[0] = current;
	negative[0] = current_negative;
	
	x = y;
	y = z;
    }
    
    if (y != 1)
	my_exit("mod_inv called with non-coprime integers " +
		std::to_string(original_x) +
		" and " +
		std::to_string(mod));
    
    return prev[0] ? negative[0] ? mod-prev[0] : prev[0] : 0;
}

// mod_inv: computes the inverse of x mod y, with x and y coprime
// if x and y are not coprime, it is as if they have been divided by their gcd
// if y == 0, this halts execution
// note that if x%y == 0, this essentially computes mod_inv(0,1) = 0.
// here x is signed
unsigned long mod_inv(long x, unsigned long y)
{
    // if long(y) < 0, y >= abs(x),
    // with equality only holding at x = long_min, y = -long_min
    // so in that case, we call mod_inv(y,y) and exit since gcd != 1 anyway
    if (long(y) > 0)
	x %= long(y);
    unsigned long x_new = x<0 ? x+y : x;
    return mod_inv(x_new,y);
}

unsigned long mod_inv(unsigned int x, unsigned long y)
{
    unsigned long new_x = x;
    return mod_inv(new_x,y);
}

unsigned long mod_inv(int x, unsigned long y)
{
    return mod_inv(long(x),y);
}



// Mod an unsigned int
class Mod
{
private:
    unsigned int mod;
    unsigned long value;
    void mod_mismatch_check(const Mod & o)
	{
	    if (mod != o.mod)
		my_exit("Mod values " +
			std::to_string(mod) +
			" and " +
			std::to_string(o.mod) +
			" do not agree");
	}
public:
    Mod(unsigned int mod, unsigned long value) :
	mod(mod),
	value(mod ? value%mod : value)
	{}
    Mod(unsigned int mod, long value) :
	mod(mod)
	{
	    if (mod)
	    {
		value %= mod;
		if (value < 0)
		    value += mod;
		this->value = value;
	    }
	    else
	    {
		if (value < 0)
		    my_exit("Mod called with mod = 0 and value (" +
			    std::to_string(value) +
			    ") < 0");
		this->value = value;
	    }
	}
    Mod(unsigned int mod, int value) : Mod(mod,long(value)) {}
    Mod(unsigned int mod, unsigned int value) : Mod(mod,(unsigned long)(value)) {}
    Mod(const Mod& o) : mod(o.mod), value(o.value) {}
    Mod(Mod&& o) { swap(o); }
    
    void swap(Mod& o)
	{
	    std::swap(mod,o.mod);
	    std::swap(value,o.value);
	}
    Mod& operator=(const Mod& o)
	{
	    mod = o.mod;
	    value = o.value;
	    return *this;
	}
    template<class T>
    Mod& operator=(const T & t)
	{
	    return *this = create(t);
	}
    Mod& operator=(Mod && o)
	{
	    swap(o);
	    return *this;
	}
    Mod operator-() const
	{
	    if (mod == 0 && value > 0)
		my_exit("-Mod called with mod = 0 and value " +
			std::to_string(value));
	    return Mod(mod,value?mod-value:0);
	}
    Mod& operator++()
	{
	    ++value;
	    if (value == mod)
		value = 0;
	    return *this;
	}
    Mod& operator--()
	{
	    if (value == 0)
	    {
		if (mod == 0)
		    my_exit("--Mod called when value and mod are 0");
		value = mod;
	    }
	    --value;
	    return *this;
	}
    Mod& operator+=(const Mod & o)
	{
	    mod_mismatch_check(o);
	    value += o.value;
	    if (value >= mod)
		value -= mod;
	    return *this;
	}
    Mod operator+(const Mod & o) const
	{
	    Mod copy(*this);
	    return copy += o;
	}
    Mod& operator-=(const Mod & o)
	{
	    mod_mismatch_check(o);
	    if (value < o.value)
		value += mod;
	    value -= o.value;
	    return *this;
	}
    Mod operator-(const Mod & o) const
	{
	    Mod copy(*this);
	    return copy -= o;
	}
    Mod& operator*=(const Mod & o)
	{
	    mod_mismatch_check(o);
	    value *= o.value;
	    if (mod)
		value %= mod;
	    return *this;
	}
    Mod operator*(const Mod & o) const
	{
	    Mod copy(*this);
	    return copy *= o;
	}
    Mod& operator/=(const Mod & o)
	{
	    mod_mismatch_check(o);
	    if (o.value == 0)
		my_exit("Tried to divide by 0");
	    
	    unsigned int gcd = my_gcd<unsigned long>(o.value,mod);
	    
	    if (value%gcd)
		my_exit("Tried to unevenly divide in Mod: " +
			std::to_string(value) +
			" by " +
			std::to_string(o.value) +
			" mod " +
			std::to_string(mod));
	    
	    value /= gcd;
	    if (mod)
	    {
		mod /= gcd;
		value *= mod_inv(o.value/gcd,mod);
		value %= mod;
	    }
	    return *this;
	}
    Mod operator/(const Mod & o) const
	{
	    Mod copy(*this);
	    return copy /= o;
	}
    Mod& operator^=(long pow)
	{
	    unsigned long real_pow = pow;
	    if (pow < 0)
	    {
		value = mod_inv(value,mod);
		real_pow = std::abs(pow);
	    }
	    Mod base = create(1);
	    swap(base);
	    for (; real_pow; real_pow>>=1)
	    {
		if (real_pow&1)
		    (*this) *= base;
		base *= base;
	    }
	    return *this;
	}
    Mod operator^(long pow) const
	{
	    Mod copy(*this);
	    return copy ^= pow;
	}
    operator bool() const
	{
	    return value > 0;
	}
    explicit operator double() const
	{
	    return value;
	}
    unsigned int get_mod() const
	{
	    return mod;
	}
    template<class T>
    Mod create(T val) const
	{
	    return Mod(mod,val);
	}
    std::string to_string() const
	{
	    std::string result = std::to_string(value);
	    if (mod)
		result += " (mod " + std::to_string(mod) + ")";
	    return result;
	}
    friend std::ostream& operator<<(std::ostream& o, Mod const & m)
	{
	    return o << m.to_string();
	}

    // operators with other data types
#define Friend_Both_Op_Decl(op,assign_op)				\
    template<class T>							\
    friend Mod& operator assign_op(Mod& m, T const & t)			\
	{ return m assign_op m.create(t); }				\
    template<class T>							\
    friend Mod operator op(Mod const & m, T const & t)			\
	{ return m op m.create(t); }					\
    template<class T>							\
    friend Mod operator op(T const & t, Mod const & m)			\
	{ return m.create(t) + m; }					\
    template<class T>							\
    friend Mod operator assign_op(Mod const & m, T const &)		\
	{ static_assert(!std::is_same<T,T>::value,			\
			"Cannot use *= on a temporary or constant Mod"); \
	    return m; }

#define Friend_Op_Decl(op) Friend_Both_Op_Decl(op,op##=)

    Friend_Op_Decl(+)
    Friend_Op_Decl(-)
    Friend_Op_Decl(*)
    Friend_Op_Decl(/)

#undef Friend_Both_Op_Decl
#undef Friend_Op_Decl
};

template<>
struct to_string_helper<Mod>
{
    static inline std::string
    to_string(Mod const & x)
	{
	    return x.to_string();
	}
};

unsigned long factorial(unsigned long x)
{
    unsigned long result = 1;
    for (; x>0; x--)
	result *= x;
    return result;
}


#endif /* MATH_UTILS */
