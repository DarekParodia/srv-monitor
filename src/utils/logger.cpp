#include "logger.h"

Logger logger;

Logger::Logger()
{
}

// private
void Logger::log_time()
{
    this->output << "\033[1;30m" << program_timer.elapsed_str() << "\033[0m";
}

// out
void Logger::log(std::string msg, LogLevel level)
{
    if (!settings::verbose || ((level == LogLevel::DEBUG || level == LogLevel::DEBUG_WARNING) && !settings::debug))
    {
        return;
    }

    // time print
    log_time();

    switch (level)
    {
    case LogLevel::INFO:
        // green
        this->output << " [\033[1;32mINFO\033[0m]";
        break;
    case LogLevel::WARNING:
        // yellow
        this->output << " [\033[1;33mWARN\033[0m]";
        break;
    case LogLevel::ERROR:
        // light red
        this->output << " [\033[1;31mERROR\033[0m]";
        break;
    case LogLevel::DEBUG:
        // light blue
        this->output << " [\033[1;36mDEBUG\033[0m]";
        break;
    case LogLevel::DEBUG_WARNING:
        // light magenta
        this->output << " [\033[1;35mDEBUG_WARN\033[0m]";
        break;
    default:
        break;
    }

    // message print
    this->output << " " << msg << std::endl;
    this->flush();
}

// in
std::string Logger::prompt(std::string msg)
{
    // time print
    log_time();

    this->output << " [\033[1;34mIN\033[0m]";
    this->output << " " << msg;

    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool Logger::confirm_prompt(std::string msg)
{
    if (settings::no_confirm)
    {
        return true;
    }
    std::string input = prompt(msg + " [Y/n] ");

    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input == "y" || input == "yes";
}