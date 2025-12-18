#pragma once
#include <string>
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <ctime>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
public:
    static Logger& Instance();

    void log(const std::string& level,
        const std::string& file,
        int line,
        const std::string& func,
        const std::string& msg);

    void stop();  // 程序退出时调用

    void StartLogger(const std::string& strPath);

private:
    Logger();
    ~Logger();

    void writeThreadFunc();
    std::string currentDate() const;
    std::string toString(LogLevel level);

private:
    std::ofstream logFile;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> logQueue;
    std::atomic<bool> running;
    std::thread writeThread;
    std::string fileDate;  // 用于按天切割日志
    std::string strFilePath;  // 用于按天切割日志
};


#define LOG_DEBUG(msg) Logger::Instance().log("DEBUG",  __FILE__, __LINE__, Q_FUNC_INFO, msg)
#define LOG_INFO(msg)  Logger::Instance().log("INFO",  __FILE__, __LINE__, Q_FUNC_INFO, msg)
#define LOG_WARN(msg)  Logger::Instance().log("WARN",  __FILE__, __LINE__, Q_FUNC_INFO, msg)
#define LOG_ERROR(msg) Logger::Instance().log("ERROR", __FILE__, __LINE__, Q_FUNC_INFO, msg)