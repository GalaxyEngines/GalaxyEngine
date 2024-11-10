//
// Created by plaidmrdeer on 2024/11/10.
//
#include "Utils.h"

#include <filesystem>
#include <iomanip>
#include <ctime>
std::string getDateTime() {
    const auto nowTime = std::chrono::system_clock::now();
    const std::time_t currentTime = std::chrono::system_clock::to_time_t(nowTime);
    const std::tm localTime = *std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    std::string dateTime = oss.str();
    return dateTime;
}