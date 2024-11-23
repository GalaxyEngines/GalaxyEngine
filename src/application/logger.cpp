#include <application/logger.h>
#include <application/utils.h>

#include <filesystem>

ge::Logger::Logger(const std::string& log_file_path_)
{
    if (!std::filesystem::exists(log_file_path_)) std::filesystem::create_directory(log_file_path_);
        output_file_ = std::ofstream(log_file_path_ + "/" + get_date_time() + ".log");
}

ge::Logger::~Logger()
{
    output_file_.close();
}

void ge::Logger::log(const LogLevel log_level_, const std::string& message_)
{
    const std::string log_message_ = "[" + get_date_time() + "] " + "[" + to_string(log_level_) + "]" + " >> " + message_;
    output_file_ << log_message_ << std::endl;
}

std::string ge::Logger::to_string (const LogLevel log_level_)
{
    switch (log_level_)
    {
        case DEBUG:    return "DEBUG";
        case INFO:     return "INFO";
        case WARNING:  return "WARNING";
        case ERROR:    return "ERROR";
        case CRITICAL: return "CRITICAL";
        default:       return " ";
    }
}
