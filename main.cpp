// homework #6: command processing (bulk).

#include <iostream>
#include <sstream>
#include <fstream>

#include "CmdProcessor.hpp"

int main()
{
    std::size_t N = 0;
    std::cin >> N;
    CmdProcessor commands{N};

    auto print = [&commands]()
    {
        std::stringstream fname;
        fname << "bulk" << commands.block_start_time(0) << ".log";
        std::fstream file{fname.str(), std::fstream::out};
        file << "bulk: ";
        std::cout << "bulk: ";

        std::size_t cntr = 0;
        for(const auto& cmd:commands.get_cmd())
        {
            file << cmd;
            std::cout << cmd;
            if(++cntr < commands.block_size())
            {
                file << ", ";
                std::cout << ", ";
            }

        }
        std::cout << '\n';
        commands.clear_commands();
    };

    std::string read;
    while(std::cin >> read)
    {
        commands.process_cmd(std::move(read));
        read.clear();
        if(commands.input_block_finished())
            print();
    }
    if(commands.remaining_data_valid())
        print();
    return 0;
}
