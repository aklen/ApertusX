#ifndef ILOGGERSERVICE_H
#define ILOGGERSERVICE_H

#include <string>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <iostream>

class ILoggerService {
public:
    /*
    * Constructor
    * Start the log thread
    * @param running Set to true to start the log thread
    */
    ILoggerService() : running(true), logThread(&ILoggerService::ProcessLogs, this) {
        std::cout << "[Logger] Log thread started!" << std::endl;
    }

    /*
    * Destructor
    * Stop the log thread
    */
    virtual ~ILoggerService() {
        running = false;
        logCondition.notify_all();
        if (logThread.joinable()) {
            logThread.join();
        }
    }

    /*
    * Log a message
    * @param message The message to log
    */
    virtual void Log(const std::string& message) {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(message);
        logCondition.notify_one();
    }

    /* 
    * Overload operator<< to allow logging with stream syntax
    * Usage: loggerService << "Log message" << std::endl;
    */
    template<typename T>
    ILoggerService& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer << value;
        return *this;
    }

    /*
    * Handle std::endl or explicit flush
    * Usage: loggerService << "Log message" << std::endl;
    */
    ILoggerService& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::lock_guard<std::mutex> lock(bufferMutex);
        Log(buffer.str());
        buffer.str("");
        buffer.clear();
        return *this;
    }

protected:
    std::stringstream buffer;
    std::mutex bufferMutex;
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable logCondition;
    bool running;
    std::thread logThread;

    /*
    * Process log messages
    * Wait for a log message to be available and print it
    */
    void ProcessLogs() {
        while (running || !logQueue.empty()) {
            std::unique_lock<std::mutex> lock(queueMutex);
            logCondition.wait(lock, [this] { return !logQueue.empty() || !running; });
            while (!logQueue.empty()) {
                std::string message = logQueue.front();
                logQueue.pop();
                lock.unlock();
                std::cout << message << std::endl;
                lock.lock();
            }
        }
    }
};

#endif // ILOGGERSERVICE_H
