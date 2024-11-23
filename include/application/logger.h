#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <string>

namespace ge
{
    enum LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,

    };

    class Logger
    {
    public:
        explicit Logger(const std::string& log_file_path_);
        ~Logger();

        void log(LogLevel log_level_, const std::string& message_);
    private:
        std::ofstream output_file_;

        static std::string to_string(LogLevel log_level_) ;
    };
}

#endif
