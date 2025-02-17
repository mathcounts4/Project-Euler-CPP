#pragma once

extern bool global_success;

int registerTest(void(*function)(), char const * test, char const * file, unsigned int line);

struct TrueFunctor {
    template<class X> bool operator()(X const& x) { return static_cast<bool>(x); }
};

struct FalseFunctor {
    template<class X> bool operator()(X const& x) { return !static_cast<bool>(x); }
};

template<class T> auto equals(T const& y) {
    return [&y](auto const& x) -> bool { return x == y; };
}

template<class T> auto isNotEqualTo(T const& y) {
    return [&y](auto const& x) -> bool { return x != y; };
}

template<class T> auto isLessThan(T const& y) {
    return [&y](auto const& x) -> bool { return x < y; };
}

template<class T> auto isLeq(T const& y) {
    return [&y](auto const& x) -> bool { return x <= y; };
}

template<class T> auto isGreaterThan(T const& y) {
    return [&y](auto const& x) -> bool { return x > y; };
}

template<class T> auto isGeq(T const& y) {
    return [&y](auto const& x) -> bool { return x >= y; };
}

#define TEST_IMPL_DETAILS(DEF_NAME,PRINT_NAME)			\
    void DEF_NAME();						\
    static auto DEF_NAME##_##registration =			\
	registerTest(DEF_NAME,PRINT_NAME,__FILE__,__LINE__);	\
    void DEF_NAME()

#define TEST_IMPL(PACKAGE,NAME)					\
    TEST_IMPL_DETAILS(t##PACKAGE##_##NAME,#PACKAGE "." #NAME)

#define VALUE_CHECK_IMPL(VALUE,CHECK_TYPE,...)				\
    do {								\
	auto disp_fail = []() -> decltype(auto) {			\
	    global_success = false;					\
	    return std::cout << CHECK_TYPE << " failed in " <<		\
		__FILE__ << ": " << __LINE__ << ": " <<			\
		#VALUE << " " << #__VA_ARGS__ << " failed due to ";	\
	};								\
	try {								\
	    auto&& value = VALUE;					\
	    if (!(__VA_ARGS__(value))) {				\
		disp_fail() << "incorrect value: " << value << std::endl; \
	    }								\
	} catch (std::exception const& ex) {				\
	    disp_fail() << "exception: " << ex.what() << std::endl;	\
	} catch (...) {							\
	    disp_fail() << "unknown exception" << std::endl;		\
	}								\
    } while (0)

#define CHECK_IMPL(VALUE,...)			\
    VALUE_CHECK_IMPL(VALUE,"CHECK",__VA_ARGS__)

#define ASSERT_IMPL(VALUE,...)						\
    do {								\
	auto prev_success = global_success;				\
	global_success = true;						\
	VALUE_CHECK_IMPL(VALUE,"ASSERT",__VA_ARGS__);			\
	if (!global_success) {						\
	    std::cout << "Terminating test due to failed assert." << std::endl; \
	    return;							\
	}								\
	global_success = prev_success;					\
    } while (0)

