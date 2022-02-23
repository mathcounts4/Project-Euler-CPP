#pragma once

#include <algorithm>
#include <cstddef>

// SmartMemory<Bytes> produces uninitialized memory on the stack or heap
// Inherit from this to use the constructor.
// All constructors do not initialize any memory.

namespace MemoryImpl {

template<std::size_t Bytes, std::size_t AlignBytes> struct StackMemory {
  protected:
    alignas(AlignBytes) char fData[std::max(Bytes, static_cast<std::size_t>(1))];
    StackMemory() {} // forcefully never initialize fData
    
};

template<std::size_t Bytes, std::size_t AlignBytes> struct HeapMemory {
  protected:
    char* const fData;
    HeapMemory()
	: fData(new (std::align_val_t(AlignBytes)) char[Bytes]) {}
    HeapMemory(HeapMemory const&)
	: HeapMemory() {}
    ~HeapMemory() {
	delete[] fData;
    }
};
    
static constexpr std::size_t MaxStackArrayBytes = 2048;
    
template<std::size_t Bytes, std::size_t AlignBytes, bool leqMaxStackArrayBytes> struct SmartMemoryImpl;
template<std::size_t Bytes, std::size_t AlignBytes> struct SmartMemoryImpl<Bytes,AlignBytes,true> {
    using Type = StackMemory<Bytes, AlignBytes>;
};
template<std::size_t Bytes, std::size_t AlignBytes> struct SmartMemoryImpl<Bytes,AlignBytes,false> {
    using Type = HeapMemory<Bytes, AlignBytes>;
};

} /* namespace MemoryImpl */

template<std::size_t Bytes, std::size_t AlignBytes> using SmartMemory = typename MemoryImpl::SmartMemoryImpl<Bytes, AlignBytes, (Bytes <= MemoryImpl::MaxStackArrayBytes)>::Type;



/* Lazy<T> */
template<class T> class Lazy : SmartMemory<sizeof(T), alignof(T)> {
  private:
    using Base = SmartMemory<sizeof(T), alignof(T)>;
    T* data() {
	return reinterpret_cast<T*>(Base::fData);
    }
    T const* data() const {
	return reinterpret_cast<T const*>(Base::fData);
    }
  public:
    operator T&() {
	return *data();
    }
    operator T const&() const {
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
