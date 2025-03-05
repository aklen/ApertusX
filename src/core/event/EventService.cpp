#include "EventService.h"
#include <iostream>

EventService::EventService(ILoggerService* logger)
    : logger(logger) {}

void EventService::Subscribe(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(eventMutex);
    subscribers[eventName].push_back(callback);
}

void EventService::Unsubscribe(const std::string& eventName, EventCallback callback) {
    // std::lock_guard<std::mutex> lock(eventMutex);
    // if (subscribers.find(eventName) != subscribers.end()) {
    //     auto& callbacks = subscribers[eventName];
    //     callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
    // }
}

void EventService::Trigger(const std::string& eventName, const std::string& param) {
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push({eventName, param});
    }
    eventCondition.notify_one();  // Wake up the worker thread
}

void EventService::Start() {
    (*logger) << "[EventService]::Start() Starting..." << std::endl;
    running = true;
    eventThread = std::thread(&EventService::EventLoop, this);
    (*logger) << "[EventService]::Start() Started." << std::endl;
}

void EventService::Stop() {
    (*logger) << "[EventService]::Stop() Notifying all threads to stop..." << std::endl;
    // Notify all threads to stop
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        running = false;
    }
    eventCondition.notify_all();  // Wake up the worker thread to allow it to exit

    (*logger) << "[EventService]::Stop() Checking if event thread should join:" << eventThread.joinable() << std::endl;
    if (eventThread.joinable()) {
        (*logger) << "[EventService]::Stop() Joining event thread..." << std::endl;
        eventThread.join();
        (*logger) << "[EventService]::Stop() Event thread joined." << std::endl;
    }
    (*logger) << "[EventService]::Stop() Stopped." << std::endl;
}

void EventService::EventLoop() {
    while (running) {
        std::unique_lock<std::mutex> lock(eventMutex);
        eventCondition.wait(lock, [this] { return !running || !eventQueue.empty(); });

        if (!running && eventQueue.empty()) {
            break;
        }

        // Process events
        while (!eventQueue.empty()) {
            auto event = eventQueue.front();
            eventQueue.pop();

            lock.unlock(); 
            if (subscribers.find(event.first) != subscribers.end()) {
                for (const auto& callback : subscribers[event.first]) {
                    callback(event.second);
                }
            }
            lock.lock();
        }
    }

    (*logger) << "[EventService]::EventLoop() Exiting..." << std::endl;
}
