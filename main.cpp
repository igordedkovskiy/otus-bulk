// homework #6: command processing (bulk).

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stack>
#include <chrono>

#include "generator.hpp"

class CmdProcessor
{
public:
    CmdProcessor() noexcept = default;
    CmdProcessor(const CmdProcessor&) = default;
    CmdProcessor(CmdProcessor&&) noexcept = default;
    CmdProcessor& operator=(const CmdProcessor&) = default;
    CmdProcessor& operator=(CmdProcessor&&) noexcept = default;
    ~CmdProcessor() noexcept = default;

    CmdProcessor(std::size_t N): m_capacity{N} { m_cmds.reserve(N); }

    bool input_block_finished() const noexcept
    {
        return m_dyn_block_finished;// && !m_braces;
    }

    std::size_t block_size() const noexcept
    {
        return m_cmds.size();
    }

    void process_cmd(std::string&& cmd)
    {
        if(cmd == "{")
        {
            if(++m_braces == 1)
            {
                if(m_cmds.size() > 0)
                    m_dyn_block_finished = true;
            }
        }
        else if(cmd == "}")
        {
            if(m_braces == 0)
                throw 1;
            if(--m_braces == 0)
                m_dyn_block_finished = true;
        }
        else
        {
            m_cmds.emplace_back(std::move(cmd));
            if(m_cmds.size() == 1)
                m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            if(m_braces == 0)
            {
                if(m_cmds.size() == m_capacity)
                    m_dyn_block_finished = true;
            }
        }
    }

    time_t block_start_time([[maybe_unused]] std::size_t number) const noexcept
    {
        return m_time;
    }

    void set_input_size(std::size_t N)
    {
        m_capacity = N;
        m_cmds.reserve(N);
    }

    void clear_commands()
    {
        m_time = 0;
        m_cmds.clear();
        m_dyn_block_finished = false;
    }

    void reset()
    {
        m_braces = 0;
        clear_commands();
    }

    generator<std::string> get_cmd() const
    {
        for(const auto& cmd:m_cmds)
            co_yield cmd;
    }

private:
    bool m_dyn_block_finished{false};
    std::size_t m_capacity{0};
    std::size_t m_braces{0};
    std::time_t m_time;
    std::vector<std::string> m_cmds;
};

int main()
{
    std::size_t N = 0;
    std::cin >> N;
    CmdProcessor cmdm{N};

    std::string read;
    while(std::cin >> read)
    {
        cmdm.process_cmd(std::move(read));
        read.clear();
        if(cmdm.input_block_finished())
        {
            std::stringstream fname;
            fname << "bulk" << cmdm.block_start_time(0) << ".log";
            std::fstream file{fname.str(), std::fstream::out};
            file << "bulk: ";
            std::cout << "bulk: ";

            std::size_t cntr = 0;
            for(const auto& cmd:cmdm.get_cmd())
            {
                file << cmd;
                std::cout << cmd;
                if(++cntr < cmdm.block_size())
                {
                    file << ", ";
                    std::cout << ", ";
                }

            }
            std::cout << '\n';
            cmdm.clear_commands();
        }
    }
    if(cmdm.block_size())
    {
        std::stringstream fname;
        fname << "bulk" << cmdm.block_start_time(0) << ".log";
        std::fstream file{fname.str(), std::fstream::out};
        file << "bulk: ";
        std::cout << "bulk: ";

        std::size_t cntr = 0;
        for(const auto& cmd:cmdm.get_cmd())
        {
            file << cmd;
            std::cout << cmd;
            if(++cntr < cmdm.block_size())
            {
                file << ", ";
                std::cout << ", ";
            }

        }
        std::cout << '\n';
        cmdm.clear_commands();
    }
    return 0;
}
