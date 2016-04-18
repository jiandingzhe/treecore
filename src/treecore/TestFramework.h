#ifndef TREEFACE_TEST_FRAMEWORK
#define TREEFACE_TEST_FRAMEWORK

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <cmath>

#include "treecore/DebugUtils.h"

#define STRINGIFY(content) #content
#define EXPAND_AND_STRINGIFY(input) STRINGIFY(input)


#define OK(...)                                        \
    {                                                  \
        bool result = __VA_ARGS__;                     \
        treecore_assert(result);                       \
        ok(result, EXPAND_AND_STRINGIFY(__VA_ARGS__)); \
    }

#define IS(_exp1_, _exp2_)                                                                    \
    {                                                                                         \
        auto _re1_ = _exp1_;                                                                  \
        auto _re2_ = _exp2_;                                                                  \
        treecore_assert(_re1_ == _re2_);                                                      \
        is( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " == " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }

#define TEST_DEFAULT_EPSILON 10000.0
#define IS_EPSILON(_exp1_, _exp2_)                                                                    \
    {                                                                                                 \
        auto _re1_ = _exp1_;                                                                          \
        auto _re2_ = _exp2_;                                                                          \
        treecore_assert(std::abs(_re1_ == _re2_) <= decltype(_re2_)(1.0 / TEST_DEFAULT_EPSILON) );    \
        is_epsilon( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " ~~ " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }
#define GT(_exp1_, _exp2_)                                                                   \
    {                                                                                        \
        auto _re1_ = _exp1_;                                                                 \
        auto _re2_ = _exp2_;                                                                 \
        treecore_assert(_re1_ > _re2_);                                                      \
        gt( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " > " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }

#define GE(_exp1_, _exp2_)                                                                    \
    {                                                                                         \
        auto _re1_ = _exp1_;                                                                  \
        auto _re2_ = _exp2_;                                                                  \
        treecore_assert(_re1_ >= _re2_);                                                      \
        ge( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " >= " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }

#define LT(_exp1_, _exp2_)                                                                   \
    {                                                                                        \
        auto _re1_ = _exp1_;                                                                 \
        auto _re2_ = _exp2_;                                                                 \
        treecore_assert(_re1_ < _re2_);                                                      \
        lt( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " < " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }

#define LE(_exp1_, _exp2_)                                                                   \
    {                                                                                        \
        auto _re1_ = _exp1_;                                                                 \
        auto _re2_ = _exp2_;                                                                 \
        treecore_assert(_re1_ <= _re2_);                                                      \
        le( _re1_, _re2_, EXPAND_AND_STRINGIFY(_exp1_) " <= " EXPAND_AND_STRINGIFY(_exp2_) ); \
    }

class TestFramework
{
public:
    TestFramework();
    TestFramework(size_t n_tests);
    ~TestFramework();

    void ok(bool value, const char* desc);

    template<typename T1, typename T2>
    void is(T1 value, T2 expect, const char* desc);

    template<typename T1, typename T2>
    void is_epsilon(T1 value, T2 expect, const char* desc, T2 rate = T2(TEST_DEFAULT_EPSILON) );

    template<typename T1, typename T2>
    void gt(T1 a, T2 b, const char* desc);

    template<typename T1, typename T2>
    void ge(T1 a, T2 b, const char* desc);

    template<typename T1, typename T2>
    void lt(T1 a, T2 b, const char* desc);

    template<typename T1, typename T2>
    void le(T1 a, T2 b, const char* desc);

    bool run();

protected:
    void content();
    size_t n_planned;
    size_t n_got;
    size_t n_fail;
};

template<typename T1, typename T2>
void TestFramework::is(T1 value, T2 expect, const char* desc)
{
    n_got++;
    if (value == expect)
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  <<"  got " << value <<", expect "<< expect << "  " << std::endl;
        n_fail++;
    }
}

template<typename T1, typename T2>
void TestFramework::is_epsilon(T1 value, T2 expect, const char* desc, T2 rate )
{
    n_got++;
    if (std::abs(value-expect) <= std::abs(1.0/rate))
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  <<"  got " << value <<", expect "<< expect << "  " << std::endl;
        n_fail++;
    }
}

template<typename T1, typename T2>
void TestFramework::gt(T1 a, T2 b, const char* desc)
{
    n_got++;
    if (a > b)
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  << "  got " << a << " and " << b << std::endl;
        n_fail++;
    }
}

template<typename T1, typename T2>
void TestFramework::ge(T1 a, T2 b, const char* desc)
{
    n_got++;
    if (a >= b)
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  << "  got " << a << " and " << b << std::endl;
        n_fail++;
    }
}

template<typename T1, typename T2>
void TestFramework::lt(T1 a, T2 b, const char* desc)
{
    n_got++;
    if (a < b)
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  << "  got " << a << " and " << b << std::endl;
        n_fail++;
    }
}

template<typename T1, typename T2>
void TestFramework::le(T1 a, T2 b, const char* desc)
{
    n_got++;
    if (a <= b)
    {
        std::cout << "ok " << n_got << " - " << desc << std::endl;
    }
    else
    {
        std::cout << "NOT ok " << n_got << " - " << desc << std::endl
                  << "  got " << a << " and " << b << std::endl;
        n_fail++;
    }
}

#endif // TREEFACE_TEST_FRAMEWORK
