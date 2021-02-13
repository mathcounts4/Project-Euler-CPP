#pragma once

template<typename T>
class Zero
{
  public:
    static inline T get()
	{
	    return T(0);
	}
};
