#include <sstream>
#include <fstream>
#include "CmdCollector.hpp"

CmdCollector::CmdCollector(std::size_t N):
    m_capacity{N}
{
    m_cmds.reserve(N);
}

bool CmdCollector::input_block_finished() const noexcept
{
    return m_block_finished;
    //return input_processor->input_block_finished();
}

void CmdCollector::finish_block() noexcept
{
    if(type == InputType::STATIC)
        m_block_finished = true;
}

std::size_t CmdCollector::block_size() const noexcept
{
    return m_cmds.size();
}

bool CmdCollector::remaining_data_valid() const noexcept
{
    if(type == InputType::STATIC)
        return block_size();
    return block_size() && m_block_finished;
}

void CmdCollector::process_cmd(std::string &&cmd)
{
    //input_processor = on_new_cmd(cmd);
    //input_processor->process_cmd(cmd);

    if(cmd == "{")
    {
        type = InputType::DYNAMIC;
        if(++m_braces == 1)
        {
            if(m_cmds.size() > 0)
                m_block_finished = true;
        }
    }
    else if(cmd == "}")
    {
        if(m_braces == 0)
            throw ParseErr::incorrect_format;
        if(--m_braces == 0)
        {
            type = InputType::STATIC;
            m_block_finished = true;
        }
    }
    else
    {
        m_cmds.emplace_back(std::move(cmd));
        if(m_cmds.size() == 1)
            m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if(m_braces == 0)
        {
            if(m_cmds.size() == m_capacity)
                m_block_finished = true;
        }
    }
}

time_t CmdCollector::block_start_time([[maybe_unused]] std::size_t number) const noexcept
{
    return m_time;
}

void CmdCollector::set_block_max_size(std::size_t N)
{
    m_capacity = N;
    m_cmds.reserve(N);
}

void CmdCollector::clear_commands()
{
    m_time = 0;
    m_cmds.clear();
    m_block_finished = false;
}

void CmdCollector::reset()
{
    m_braces = 0;
    clear_commands();
}

generator<std::string> CmdCollector::get_cmd() const
{
//    if(type == InputType::DYNAMIC)
//    {
//        if(!m_block_finished)
//            co_return;
//    }
    for(const auto& cmd:m_cmds)
        co_yield cmd;
    //input_processor->get_cmd();
}

//InputType* on_new_cmd(const std::string& cmd)
//{
//    if(cmd == "{")
//    {
//        delete input_processor;
//        input_processor = new Dynamic;
//    }
//    else if(cmd == "}" and --m_braces == 0)
//    {
//        delete input_processor;
//        input_processor = new Static;
//    }
//    return input_processor;
//}

//generator<std::string> CmdProcessor::InputType::get_cmd() const
//{
//    for(const auto& cmd:m_cmds)
//        co_yield cmd;
//}

//void CmdProcessor::Static::process_cmd(std::string &&cmd)
//{
//    m_cmds.emplace_back(std::move(cmd));
//    if(m_cmds.size() == 1)
//        m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//    if(m_cmds.size() == m_capacity)
//        m_block_finished = true;
//}

//void CmdProcessor::Static::finish_input() noexcept override
//{
//    ;
//}

//generator<std::string> CmdProcessor::Dynamic::get_cmd() const
//{
//    if(m_block_finished)
//        return InputType::get_cmd();
//}

//void CmdProcessor::Dynamic::process_cmd(std::string &&cmd)
//{
//    if(cmd == "{")
//    {
//        ++m_braces;
//    }
//    else if(cmd == "}")
//    {
//        if(m_braces == 0)
//            throw 1;
//        if(--m_braces == 0)
//            m_dyn_block_finished = true;
//    }
//    else
//    {
//        m_cmds.emplace_back(std::move(cmd));
//        if(m_cmds.size() == 1)
//            m_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//    }
//}
