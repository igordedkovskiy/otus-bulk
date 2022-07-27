// homework #6: command processing (bulk).

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "CmdCollector.hpp"
#include "read_input.hpp"

int main()
{
    CmdCollector commands{read_input_size(std::cin, std::cerr)};

    auto print = [](std::ostream& stream, CmdCollector::cmds_t cmds,
            std::mutex& m, std::condition_variable& cv, std::atomic<bool>& v)
    {
        {
            std::unique_lock lk{m};
            std::cout << '\n';
            std::cout << __PRETTY_FUNCTION__ << '\n';
            std::cout << std::this_thread::get_id() << std::endl;
            stream << "bulk: ";
            std::size_t cntr = 0;
            for(const auto& cmd:cmds)
            {
                stream << cmd;
                if(++cntr < cmds.size())
                    stream << ", ";
            }
            stream << '\n';
            v = true;
        }
        cv.notify_all();
    };

    std::mutex m1, m2, ml;
    std::condition_variable cv1, cv2, cvl;
    std::atomic<bool> v1{true}, v2{true}, vl{true};
    std::thread f1, f2, log;

    std::stringstream fname;
    std::fstream file1, file2;

    std::size_t fcntr{0};

    auto process = [&](std::string&& read)
    {
        commands.process_cmd(std::move(read));
        read.clear();
        if(commands.input_block_finished())
        {
            const auto cmds{std::move(commands.get_cmds())};

            if(!vl)
            {
                std::unique_lock lk(ml);
                cvl.wait(lk);
            }
            {
                log = std::thread{print, std::ref(std::cout), cmds, std::ref(ml), std::ref(cvl), std::ref(vl)};
                vl = false;
                log.detach();
            }

            fname.str(std::string{});
            fname << "bulk-" << commands.block_start_time(0) << '-' << ++fcntr << ".log";

            if(v1)
            {
                file1 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f1 = std::thread{print, std::ref(file1), std::move(cmds), std::ref(m1), std::ref(cv1), std::ref(v1)};
                v1 = false;
                f1.detach();
            }
            else if(v2)
            {
                file2 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f2 = std::thread{print, std::ref(file2), std::move(cmds), std::ref(m2), std::ref(cv2), std::ref(v2)};
                v2 = false;
                f2.detach();
            }
            else
            {
                if(!v1)
                {
                    std::unique_lock lk(m1);
                    cv1.wait(lk);
                }
                file1 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f1 = std::thread{print, std::ref(file1), std::move(cmds), std::ref(m1), std::ref(cv1), std::ref(v1)};
                v1 = false;
                f1.detach();
            }

            commands.clear_commands();
        }
    };

    read_input<decltype(process), CmdCollector::ParseErr>(std::cin, std::cerr, process);
    commands.finish_block();
    process("{");

    if(!v1)
    {
        std::unique_lock lk(m1);
        cv1.wait(lk);
    }
    if(!v2)
    {
        std::unique_lock lk(m2);
        cv2.wait(lk);
    }
    if(!vl)
    {
        std::unique_lock lk(ml);
        cvl.wait(lk);
    }

    return 0;
}
