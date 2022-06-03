// homework #6: command processing (bulk).

#include <iostream>
#include <vector>
#include <chrono>
//#include "infinite_matrix.hpp"

// TODO: change IsBalanced according to task to manage blocks, bounded by curly braces
// IsBalanced(str)
// Stack stack
// for char in str:
//     if char in [‘(‘, ‘[‘]:
//         stack.Push(char )
//     else
//         if stack.Empty(): return False
//             top ← stack.Pop()
//         if (top = ‘[‘ and char != ‘]’) or (top = ‘(‘ and char != ‘)’):
//             return False
// return stack.Empty()

int main()
{
    std::size_t N = 0;
    std::cin >> N;
    std::string read;
    std::vector<std::string> cmds;
    cmds.reserve(N);

    std::cin >> read;
    cmds.emplace_back(std::move(read));

    std::time_t newt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "newt: " << newt << '\n';
    std::cout << "newt: " << std::ctime(&newt) << '\n';

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
