#include <iostream>

const long MOD_DEFAULT = 1e9+7;

template<unsigned int mod = MOD_DEFAULT>
class Mod
{
  private:
    unsigned long m_data;

  public:
    inline unsigned long get() const { return m_data; }

    Mod() : m_data(0) {}
    Mod(unsigned long value) : m_data(value % mod) {}
    Mod(const Mod& other) : m_data(other.get()) {}

    Mod& operator=(const Mod& other)
	{
	    m_data = other.get();
	}

    Mod& operator=(Mod&& other)
	{
	    m_data = other.get();
	}

    Mod operator-() const
	{
	    return Mod(mod - m_data);
	}

    Mod& operator+=(const Mod& other)
	{
	    m_data += other.get();
	    if (m_data >= mod)
	    {
		m_data -= mod;
	    }
	    return *this;
	}

    Mod& operator-=(const Mod& other)
	{
	    if (m_data < other.get())
	    {
		m_data += mod;
	    }
	    m_data -= other.get();
	    return *this;
	}

    Mod operator+(const Mod& other) const
	{
	    Mod result(m_data + other.get());
	    std::cout << "hello" << std::endl;
	    return result;
	}

    Mod operator+(Mod&& other) const
	{
	    other += (*this);
	    std::cout << "hi" << std::endl;
	    return other;
	}
};

template<unsigned int mod>
std::ostream& operator<<(std::ostream& s, const Mod<mod>& m)
{
    return s << "(" << m.get() << " mod " << mod << ")";
}
