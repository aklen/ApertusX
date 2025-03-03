#include "EventService.h"
#include <iostream>

void EventService::Subscribe(const std::string& eventName, EventCallback callback) {
    std::lock_guard<std::mutex> lock(eventMutex);
    subscribers[eventName].push_back(callback);
}

void EventService::Trigger(const std::string& eventName, const std::string& param) {
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push({eventName, param});
    }
    eventCondition.notify_one();  // Wake up the worker thread
}

void EventService::Start() {
    running = true;
    eventThread = std::thread(&EventService::EventLoop, this);
}

void EventService::Stop() {
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        running = false;
    }
    eventCondition.notify_all();  // Wake up the worker thread to allow it to exit

    // 🚀 Process remaining events before shutting down
    while (true) {
        std::pair<std::string, std::string> event;
        {
            std::lock_guard<std::mutex> lock(eventMutex);
            if (eventQueue.empty()) {
                break;  // Exit if no more events to process
            }
            event = eventQueue.front();
            eventQueue.pop();
        }

        if (subscribers.find(event.first) != subscribers.end()) {
            for (const auto& callback : subscribers[event.first]) {
                callback(event.second);
            }
        }
    }

    if (eventThread.joinable()) {
        eventThread.join();
    }
}

void EventService::EventLoop() {
    while (running) {
        std::pair<std::string, std::string> event;
        {
            std::unique_lock<std::mutex> lock(eventMutex);
            eventCondition.wait(lock, [this] { return !eventQueue.empty() || !running; });

            if (!running && eventQueue.empty()) {
                break;
            }

            event = eventQueue.front();
            eventQueue.pop();
        }

        // Call the event handlers
        std::lock_guard<std::mutex> lock(eventMutex);
        if (subscribers.find(event.first) != subscribers.end()) {
            for (const auto& callback : subscribers[event.first]) {
                callback(event.second);
            }
        }
    }
}
