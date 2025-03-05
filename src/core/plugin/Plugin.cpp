#include "Plugin.h"
#include <iostream>

Plugin::Plugin(IEventService* eventService, ILoggerService* logger)
    : eventService(eventService), logger(logger), running(false) {}

Plugin::~Plugin() {
    (*logger) << "[Plugin] Destructor called." << std::endl;
    Destroy();
}

void Plugin::Init() {
    (*logger) << "[Plugin] Base Plugin initialized, starting event listener thread." << std::endl;

    running = true;
    eventListenerThread = std::thread(&Plugin::EventProcessingLoop, this);
}

void Plugin::Run() {
    (*logger) << "[Plugin] Running on thread ID: " << std::this_thread::get_id() << std::endl;
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulated work
    }
}

void Plugin::Destroy() {
    running = false;
    eventCondition.notify_all();

    if (eventListenerThread.joinable()) {
        eventListenerThread.join();
    }
}

void Plugin::subscribe(const std::string& eventName, std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(eventMutex);
    eventCallbacks[eventName] = callback;  // 🔥 Eltároljuk a callback függvényt

    eventService->Subscribe(eventName, [this, eventName](const std::string& param) {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push({eventName, param});
        eventCondition.notify_one();
    });

    (*logger) << "[Plugin] Subscribed to event: " << eventName << std::endl;
}

void Plugin::EventProcessingLoop() {
    (*logger) << "[Plugin] Event processing thread started." << std::endl;

    while (running) {
        std::unique_lock<std::mutex> lock(eventMutex);
        eventCondition.wait(lock, [this] { return !running || !eventQueue.empty(); });

        while (!eventQueue.empty()) {
            auto event = eventQueue.front();
            eventQueue.pop();
            lock.unlock();

            (*logger) << "[Plugin] Processing event: " << event.first << " with data: " << event.second << std::endl;

            // 🔥 Meg kell hívni az eseményhez tartozó callback függvényt
            auto it = eventCallbacks.find(event.first);
            if (it != eventCallbacks.end()) {
                (*logger) << "[Plugin] Calling event callback for: " << event.first << std::endl;
                it->second(event.second);  // Meghívjuk a callback függvényt
            } else {
                (*logger) << "[Plugin] No callback found for event: " << event.first << std::endl;
            }

            lock.lock();
        }
    }

    (*logger) << "[Plugin] Event processing thread exiting." << std::endl;
}
