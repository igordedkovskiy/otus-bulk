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

    struct sync_context
    {
        void run(decltype(print)& print, std::ostream& stream, CmdCollector::cmds_t cmds)
        {
            m_thread = std::thread{print, std::ref(stream), std::move(cmds), std::ref(m_mutex), std::ref(m_cv), std::ref(m_done)};
            m_done = false;
            m_thread.detach();
        }

        void wait()
        {
            if(!m_done)
            {
                std::unique_lock lk(m_mutex);
                m_cv.wait(lk);
            }
        }

        bool ready() const noexcept
        {
            return m_done;
        }

        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::atomic<bool> m_done{true};
        std::thread m_thread;
    };

    sync_context f1, f2, log;

    std::fstream file1, file2;

    std::size_t fcntr{0};

    auto process = [&](std::string&& read)
    {
        commands.process_cmd(std::move(read));
        read.clear();
        if(commands.input_block_finished())
        {
            const auto cmds{std::move(commands.get_cmds())};

            log.wait();
            log.run(print, std::cout, cmds);

            std::stringstream fname;
            fname << "bulk-" << commands.block_start_time(0) << '-' << ++fcntr << ".log";

            if(f1.ready())
            {
                file1 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f1.run(print, file1, std::move(cmds));
            }
            else if(f2.ready())
            {
                file2 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f2.run(print, file2, std::move(cmds));
            }
            else
            {
                f1.wait();
                file1 = std::fstream{fname.str(), std::fstream::out | std::fstream::app};
                f1.run(print, file1, std::move(cmds));
            }

            commands.clear_commands();
        }
    };

    read_input<decltype(process), CmdCollector::ParseErr>(std::cin, std::cerr, process);
    commands.finish_block();
    process("{");

    f1.wait();
    f2.wait();
    log.wait();

    return 0;
}
