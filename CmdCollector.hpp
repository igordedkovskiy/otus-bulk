#pragma once

#include <vector>
#include <string>
#include <chrono>
#include "generator.hpp"

class CmdCollector
{
public:
    enum class ParseErr: std::uint8_t
    {
        incorrect_format
    };

    CmdCollector() noexcept = default;
    CmdCollector(const CmdCollector&) = default;
    CmdCollector(CmdCollector&&) noexcept = default;
    CmdCollector& operator=(const CmdCollector&) = default;
    CmdCollector& operator=(CmdCollector&&) noexcept = default;
    ~CmdCollector() noexcept = default;

    CmdCollector(std::size_t N);

    bool input_block_finished() const noexcept;
    void finish_block() noexcept;

    std::size_t block_size() const noexcept;

    bool remaining_data_valid() const noexcept;

    void process_cmd(std::string&& cmd);

    time_t block_start_time([[maybe_unused]] std::size_t number) const noexcept;

    void set_block_max_size(std::size_t N);

    void clear_commands() noexcept;

    void reset() noexcept;

    generator<std::string> get_cmd() const;

private:
//    InputType* on_new_cmd(const std::string& cmd);
//    struct InputType
//    {
//        virtual ~InputType() noexcept = 0;
//        virtual void process_cmd(std::string&& cmd) = 0;
//        virtual void finish_input() noexcept {}
//        virtual generator<std::string> get_cmd() const;
//    };
//    struct Static: public InputType
//    {
//        void process_cmd(std::string&& cmd) override;
//        void finish_input() noexcept;
//    };
//    struct Dynamic: public InputType
//    {
//        generator<std::string> get_cmd() const override;
//        void process_cmd(std::string&& cmd) override;
//    };

    enum class InputType { STATIC, DYNAMIC };

    InputType m_type{InputType::STATIC};
    bool m_block_finished{false};
    std::size_t m_capacity{0};
    std::size_t m_braces{0};
    std::time_t m_time;
    std::vector<std::string> m_cmds;
};
