#pragma once

template<typename T>
class One
{
  public:
    static inline T get()
	{
	    return T(1);
	}
};
