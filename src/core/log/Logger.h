#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <string>

namespace GE {
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,

    };

    class Logger {
    public:
        explicit Logger(const std::string& logFilePath);
        ~Logger();

        void log(LogLevel logLevel, const std::string& message);
    private:
        std::ofstream outputFile;

        std::string toString(LogLevel logLevel) const;
    };
}

#endif
