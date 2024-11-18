#include "logger.h"
#include "utils/Utils.h"

#include <filesystem>

GE::Logger::Logger(const std::string& logFilePath)
{
    if (!std::filesystem::exists(logFilePath)) std::filesystem::create_directory(logFilePath);
    outputFile = std::ofstream(logFilePath + "/" + getDateTime() + ".log");
}

GE::Logger::~Logger()
{
    outputFile.close();
}

void GE::Logger::log(const LogLevel logLevel, const std::string& message)
{
    const std::string logMessage = "[" + getDateTime() + "] " + "[" + toString(logLevel) + "]" + " >> " + message;
    outputFile << logMessage << std::endl;
}

std::string GE::Logger::toString (const LogLevel logLevel) const
{
    switch (logLevel)
    {
        case DEBUG:    return "DEBUG";
        case INFO:     return "INFO";
        case WARNING:  return "WARNING";
        case ERROR:    return "ERROR";
        case CRITICAL: return "CRITICAL";
        default:       return " ";
    }
}
