#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

// ============================================================
// 简单日志工具
//
// 用法：
//     LOG_INFO("user login: " + std::to_string(userId));
//     LOG_WARN("invalid loan amount");
//     LOG_ERROR("database connection failed");
// ============================================================

class Logger
{
public:
    enum class Level
    {
        INFO,
        WARN,
        ERROR_LEVEL
    };

    static std::string levelToString(Level level)
    {
        switch (level)
        {
            case Level::INFO:
                return "INFO";
            case Level::WARN:
                return "WARN";
            case Level::ERROR_LEVEL:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    static std::string currentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static void log(Level level, const std::string& message)
    {
        std::cout << "[" << currentTime() << "]"
                  << "[" << levelToString(level) << "] "
                  << message
                  << std::endl;
    }
};

#ifdef LOG_INFO
#undef LOG_INFO
#endif
#ifdef LOG_WARN
#undef LOG_WARN
#endif
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif

#define LOG_INFO(msg)  Logger::log(Logger::Level::INFO,  msg)
#define LOG_WARN(msg)  Logger::log(Logger::Level::WARN,  msg)
#define LOG_ERROR(msg) Logger::log(Logger::Level::ERROR_LEVEL, msg)
