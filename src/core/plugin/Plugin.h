#ifndef PLUGIN_H
#define PLUGIN_H

#include "interfaces/IPlugin.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class Plugin : public IPlugin {
public:
    Plugin(IEventService* eventService, ILoggerService* logger);
    virtual ~Plugin();

    void Init() override;
    void Run() override;
    void Destroy() override;

    std::string GetName() const override = 0;
    std::thread::id GetThreadId() const override = 0;

protected:
    void subscribe(const std::string& eventName, std::function<void(const std::string&)> callback);

    IEventService* eventService;
    ILoggerService* logger;
    std::atomic<bool> running;
    
private:
    void EventProcessingLoop();
    
    std::queue<std::pair<std::string, std::string>> eventQueue;
    std::unordered_map<std::string, std::function<void(const std::string&)>> eventCallbacks;
    std::mutex eventMutex;
    std::condition_variable eventCondition;
    std::thread eventListenerThread;
};

#endif // PLUGIN_H
