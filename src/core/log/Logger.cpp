#include "Logger.h"

#include <filesystem>
#include <iomanip>
#include <ctime>
std::string getDateTime();
std::string logLevelToString(GE::LogLevel logLevel);

GE::Logger::Logger(const std::string& logFilePath) {
    if (!std::filesystem::exists(logFilePath)) std::filesystem::create_directory(logFilePath);
    outputFile = std::ofstream(logFilePath + "/" + getDateTime() + ".log");
}

GE::Logger::~Logger() {
    outputFile.close();
}

void GE::Logger::log(const LogLevel logLevel, const std::string& message)  {
    const std::string logMessage = "[" + getDateTime() + "] " + "[" + logLevelToString(logLevel) + "]" + " >> " + message;
    outputFile << logMessage << std::endl;
}

std::string getDateTime() {
    const auto nowTime = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(nowTime);
    const std::tm localTime = *std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    std::string dateTime = oss.str();
    return  dateTime;
}

std::string logLevelToString(const GE::LogLevel logLevel) {
    switch (logLevel) {
        case GE::LogLevel::TRACE:    return "TRACE";
        case GE::LogLevel::DEBUG:    return "DEBUG";
        case GE::LogLevel::INFO:     return "INFO";
        case GE::LogLevel::WARNING:  return "WARNING";
        case GE::LogLevel::ERROR:    return "ERROR";
        case GE::LogLevel::CRITICAL: return "CRITICAL";
    }
    return "";
}
