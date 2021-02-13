#pragma once

// defines a map from unsigned integers to values,
// but when modifying the map, can modify all numbers >= a key,
// adding the same value, all at once

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <vector>

template<class Value>
class Map_GEQ_Sum
{
private:
    unsigned int max;
    Value start;
    std::vector<std::vector<Value> > binary_value_storage;
public:
    // maximum allowed index
    Map_GEQ_Sum(unsigned int l_max, Value zero = 0) :
	max(l_max),
	start(zero)
	{
	    for (; l_max > 0; l_max >>= 1)
		binary_value_storage.push_back(
		    std::vector<Value>((l_max>>1)+(l_max&1),zero));
	}
    // O(log(x))
    Value operator[](unsigned int x) const
	{   
	    if (x > max)
	        x = max;
	    
	    Value result = start;
	    
	    for (std::size_t bit = 0; x > 0; ++bit, x >>= 1)
		if (x&1)
		    result += binary_value_storage[bit][x>>1];
	    
	    return result;
	}
    // O(log(max))
    void addGEQ(unsigned int x, Value const & v)
	{
	    if (x == 0)
	    {
		start += v;
	    }
	    else if (x <= max)
	    {
	        for (std::size_t bit = 0; (max >> bit) > 0; ++bit)
		{
		    unsigned int lower = (x>>(bit+1))<<(bit+1);
		    unsigned int upper = lower | (1U<<bit);
		    if (lower < x && x <= upper && upper <= max)
			binary_value_storage[bit][x>>(bit+1)] += v;
		}
	    }
	}
    // O(log(max))
    void addRange(unsigned int x, unsigned int y, Value const & v)
	{
	    if (y < x)
		std::swap(x,y);
	    addGEQ(x,v);
	    if (y+1 > y)
		addGEQ(y+1,-v);
	}
    // O(log(max))
    void addTo(unsigned int x, Value const & v)
	{
	    addRange(x,x,v);
	}
};
