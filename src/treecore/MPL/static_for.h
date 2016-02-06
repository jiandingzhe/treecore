#include <type_traits>
#include <tuple>
#include <functional>

using namespace std;

namespace treecore {
namespace mpl {

template<typename T, int loop_num>
struct static_for
{
    void operator()()
    {
        static_for<loop_num-1>::operator()();
        T::run();
    }
};
template<>
struct static_for<0>
{
    void operator()() {};
};


long fun( int , int ) {};

// test code below:
int main()
{
    auto lambda = []( int i , float k ) { return long( i * 10 ); };
    std::function<long( int , int )> k;
    
    
    
    return 0;
}


}
}