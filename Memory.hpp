#pragma once

#include <cstddef>

// SmartMemory<Bytes> produces uninitialized memory on the stack or heap
// Inherit from this to use the constructor.
// All constructors do not initialize any memory.

namespace MemoryImpl {

template<std::size_t Bytes> struct StackMemory {
  protected:
    char fData[Bytes];
    StackMemory() {} // forcefully never initialize fData
    
};

template<std::size_t Bytes> struct HeapMemory {
  protected:
    char* const fData;
    HeapMemory()
	: fData(new char[Bytes]) {}
    HeapMemory(HeapMemory const &)
	: HeapMemory() {}
    ~HeapMemory() {
	delete[] fData;
    }
};

static constexpr std::size_t MaxStackArrayBytes = 2048;

template<std::size_t Bytes, bool leqMaxStackArrayBytes> struct SmartMemoryImpl;
template<std::size_t Bytes> struct SmartMemoryImpl<Bytes,true> {
    using Type = StackMemory<Bytes>;
};
template<std::size_t Bytes> struct SmartMemoryImpl<Bytes,false> {
    using Type = HeapMemory<Bytes>;
};

} /* namespace MemoryImpl */

template<std::size_t Bytes> using SmartMemory = typename MemoryImpl::SmartMemoryImpl<Bytes,(Bytes <= MemoryImpl::MaxStackArrayBytes)>::Type;



/* Lazy<T> */
template<class T> class Lazy : SmartMemory<sizeof(T)> {
  private:
    T * data() {
	return reinterpret_cast<T*>(SmartMemory<sizeof(T)>::fData);
    }
    T const * data() const {
	return reinterpret_cast<T const *>(SmartMemory<sizeof(T)>::fData);
    }
  public:
    operator T&() {
	return *data();
    }
    operator T const &() const {
	return *data();
    }
    template<class... Args>
    void construct(Args&&... args) {
	new(data()) T(static_cast<Args&&>(args)...);
    }
    void destroy() {
	data()->~T();
    }
};
