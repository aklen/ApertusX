#ifndef PLUGINSERVICE_H
#define PLUGINSERVICE_H

#include "interfaces/IPluginService.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <vector>
#include <memory>
#include <thread>
#include <fruit/fruit.h>

class PluginService : public IPluginService {
public:
    INJECT(PluginService(IEventService* eventService, ILoggerService* logger));
    ~PluginService() override;

    void RegisterPlugin(std::shared_ptr<IPlugin> plugin) override;
    void InitPlugins() override;
    void StartPlugins() override;
    void StopPlugins() override;

    // static void SignalHandler(int signal);

private:
    IEventService* eventService;
    ILoggerService* logger;
    std::vector<std::shared_ptr<IPlugin>> plugins;
    std::vector<std::thread> pluginThreads;
    // static std::atomic<bool> running;
    std::atomic<int> initializedPlugins{0};
    std::atomic<int> totalPlugins{0};
    std::condition_variable initCondition;
    std::mutex initMutex;
    bool shutdownCalled = false;
    std::mutex shutdownMutex;
};

#endif // PLUGINSERVICE_H
