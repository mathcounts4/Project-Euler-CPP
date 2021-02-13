#pragma once

// Checks that EXPR (parametrized by name ID)
// would NOT compile with TYPE substituted for ID in EXPR
// If it would compile, static asserts with a nice message

// For now, can only be used outside functions (and classes?)


/* Usage (EXPR would not compile, does not assert)

template<class> struct S {
    void foo();
};
ASSERT_FAIL_COMPILE(declval<S<T> const>().foo(),T,int);

*/


/* Usage (EXPR would compile, asserts)

template<class> struct S {
    void foo() const;
};
ASSERT_FAIL_COMPILE(declval<S<T> const>().foo(),T,int);

*/



#define ASSERT_FAIL_COMPILE(EXPR,ID,TYPE)			\
    ASSERT_FAIL_COMPILE_IMPL_1(					\
	EXPR,							\
	ID,							\
	TYPE,							\
	__LINE__,						\
	"Expected \"" #EXPR "\" to fail with " #ID " = " #TYPE)

#define ASSERT_FAIL_COMPILE_IMPL_1(EXPR,ID,TYPE,LINE,MSG)	\
    ASSERT_FAIL_COMPILE_IMPL_2(EXPR,ID,TYPE,compile_at_##LINE,MSG)

#define ASSERT_FAIL_COMPILE_IMPL_2(EXPR,ID,TYPE,VAR,MSG)		\
    struct Wrapper {							\
	template<class T, class = void>					\
	static constexpr bool VAR = false;				\
	template<class ID>						\
	static constexpr bool VAR<ID,std::void_t<decltype(EXPR)> > = true; \
	static_assert(!VAR<TYPE>,MSG);					\
    }
