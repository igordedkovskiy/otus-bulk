// homework #6: command processing (bulk).

#include <iostream>
#include <vector>
#include <chrono>
//#include "infinite_matrix.hpp"

int main()
{
    std::size_t N = 0;
    std::cin >> N;
    std::string read;
    std::vector<std::string> cmds;
    cmds.reserve(N);

    std::cin >> read;
    cmds.emplace_back(std::move(read));
    //auto start = std::chrono::sys_time<std::chrono::seconds>;
    //auto start = std::chrono::system_clock::now();
    //std::cout << "Current time: " << std::chrono::duration_cast<std::chrono::milliseconds>(start).count() << std::end;

    while(std::cin >> read)
    {
        cmds.emplace_back(std::move(read));
        if(cmds.size() == N)
        {
            for(const auto& cmd:cmds)
                std::cout << cmd << ' ';
            std::cout << '\n';
            cmds.clear();
        }
    }
    for(const auto& cmd:cmds)
        std::cout << cmd << ' ';
    std::cout << '\n';
    return 0;
}
