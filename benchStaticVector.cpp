
 // benchmark based on: http://cpp-next.com/archive/2010/10/howards-stl-move-semantics-benchmark/

#include "StaticVector.hpp"
#include <vector>
#include <iostream>
#include <time.h>
#include <set>
#include <algorithm>
#include <exception>
 
const unsigned N = 3001;
 
extern bool some_test;
 
template<typename T>
T get_set(std::size_t)
{
    T s;
    for (std::size_t i = 0; i < N; ++i)
        while (!s.insert(std::rand()).second)
            ;
    if (some_test)
        return s;
    return T();
}
 
template<typename T>
T generate()
{
    T v;
    for (std::size_t i = 0; i < N; ++i)
        v.push_back(get_set<typename T::value_type>(i));
    if (some_test)
        return v;
    return T();
}
 
template<typename T>
float time_it()
{
    clock_t t1, t2, t3, t4;
    clock_t t0 = clock();
    {
    T v = generate<T>();
    t1 = clock();
    std::cout << "construction took " << (float)((t1 - t0)/(double)CLOCKS_PER_SEC) << std::endl;
    std::sort(v.begin(), v.end());
    t2 = clock();
    std::cout << "sort took " << (float)((t2 - t1)/(double)CLOCKS_PER_SEC) << std::endl;
    std::rotate(v.begin(), v.begin() + v.size()/2, v.end());
    t3 = clock();
    std::cout << "rotate took " << (float)((t3 - t2)/(double)CLOCKS_PER_SEC) << std::endl;
    }
    t4 = clock();
    std::cout << "destruction took " << (float)((t4 - t3)/(double)CLOCKS_PER_SEC) << std::endl;
    std::cout << "done" << std::endl;
    return (float)((t4-t0)/(double)CLOCKS_PER_SEC);
}

int main()
{
  try {
    std::cout << "N = " << N << "\n\n";
    
    std::cout << "StaticVector Benchmark:\n";
    float t = time_it<boost::StaticVector<std::set<std::size_t>,N > >();
    std::cout << "Total time = " << t << "\n\n";
    
    std::cout << "Vector: Benchmark\n";
    t = time_it<std::vector<std::set<std::size_t> > >();
    std::cout << "Total time = " << t << '\n';
  }catch(std::exception e){
    std::cout << e.what();
  }
}
 
bool some_test = true;
