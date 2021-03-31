
#ifndef FMU_PROXY_EXAMPLE_UTIL_HPP
#define FMU_PROXY_EXAMPLE_UTIL_HPP

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace fmuproxy
{

void wait_for_input()
{
    do {
        std::cout << '\n'
                  << "Press a key to continue...\n";
    } while (std::cin.get() != '\n');
    std::cout << "Done." << std::endl;
}

template<typename function>
inline float measure_time_sec(function&& fun)
{
    auto t_start = std::chrono::high_resolution_clock::now();
    fun();
    auto t_stop = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(t_stop - t_start).count();
}

} // namespace fmuproxy

#endif //FMU_PROXY_EXAMPLE_UTIL_HPP
