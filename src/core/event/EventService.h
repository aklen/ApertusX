#ifndef EVENTSERVICE_H
#define EVENTSERVICE_H

#include "interfaces/IEventService.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <fruit/fruit.h>

class EventService : public IEventService {
public:
    INJECT(EventService()) = default;

    void Subscribe(const std::string& event, EventCallback callback) override;
    void Trigger(const std::string& event, const std::string& param = "") override;
    void Start() override;
    void Stop() override;

private:
    std::unordered_map<std::string, std::vector<EventCallback>> subscribers;
    std::queue<std::pair<std::string, std::string>> eventQueue;
    std::mutex eventMutex;
    std::condition_variable eventCondition;
    std::thread eventThread;
    std::atomic<bool> running;

    void EventLoop();
};

#endif // EVENTSERVICE_H
