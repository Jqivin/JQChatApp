#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : running(true)
{
    fileDate = currentDate();

    
}

Logger::~Logger()
{
    stop();
}

std::string Logger::currentDate() const
{
    time_t t = time(nullptr);
    struct tm tmInfo;
#ifdef _WIN32
    localtime_s(&tmInfo, &t);
#else
    localtime_r(&t, &tmInfo);
#endif

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tmInfo);
    return std::string(buf);
}

std::string Logger::toString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO:  return "INFO";
    case LogLevel::WARN:  return "WARN";
    case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}


void Logger::log(const std::string& level,
    const std::string& file,
    int line,
    const std::string& func,
    const std::string& msg)
{
    // 获取时间戳
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char timebuf[32];
#ifdef _WIN32
    ctime_s(timebuf, sizeof(timebuf), &t);
#else
    ctime_r(&t, timebuf);
#endif

    // 去掉末尾换行
    std::string timeStr(timebuf);
    timeStr.pop_back();
    int iLast = file.find_last_of("\\");
    std::string filename = file.substr(iLast, file.length() - iLast);

    std::string logLine =
        "[" + timeStr + "] [" + level + "] [" + filename + "] [" + func + ":" + std::to_string(line) + "]" +msg + "\n";

    {
        std::lock_guard<std::mutex> lock(mtx);
        logQueue.push(logLine);
    }
    cv.notify_one();
}
void Logger::writeThreadFunc()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !logQueue.empty() || !running; });

        while (!logQueue.empty())
        {
            // 每天自动切割
            if (currentDate() != fileDate)
            {
                fileDate = currentDate();
                logFile.close();
                logFile.open("client_" + fileDate + ".log", std::ios::app);
            }

            std::string msg = logQueue.front();
            logQueue.pop();

            logFile << msg;
            logFile.flush();  // 立刻写入
        }
    }
}


void Logger::stop()
{
    if (!running) return;
    running = false;
    cv.notify_one();
    if (writeThread.joinable())
        writeThread.join();
    if (logFile.is_open())
        logFile.close();
}

void Logger::StartLogger(const std::string& strPath)
{
    std::string filename = strPath + "/JQChat_" + fileDate + ".log";

    // 创建日志文件
    logFile.open(filename, std::ios::app);

    // 启动日志线程
    writeThread = std::thread(&Logger::writeThreadFunc, this);
}
