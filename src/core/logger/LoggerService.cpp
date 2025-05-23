#include "LoggerService.h"
#include <iostream>

// void LoggerService::Log(const std::string& message) {
//     std::lock_guard<std::mutex> guard(logMutex);
//     std::cout << "[Logger] " << message << std::endl;
// }

void LoggerService::Log(const std::string& message) {
    std::lock_guard<std::mutex> guard(queueMutex);
    logQueue.push(message);
    logCondition.notify_one();
}
