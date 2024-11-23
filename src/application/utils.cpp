//
// Created by plaidmrdeer on 2024/11/10.
//
#include <application/utils.h>

#include <filesystem>
#include <iomanip>
#include <ctime>

std::string get_date_time()
{
    const auto now_time_ = std::chrono::system_clock::now();
    const std::time_t current_time_ = std::chrono::system_clock::to_time_t(now_time_);
    const std::tm local_time_ = *std::localtime(&current_time_);

    std::ostringstream oss_;
    oss_ << std::put_time(&local_time_, "%Y-%m-%d %H:%M:%S");
    std::string date_time_ = oss_.str();
    return date_time_;
}