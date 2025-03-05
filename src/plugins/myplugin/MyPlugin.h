#ifndef MYPLUGIN_H
#define MYPLUGIN_H

#include "interfaces/IPlugin.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <memory>
#include <atomic>
#include <fruit/fruit.h>

class MyPlugin : public IPlugin {
public:
    INJECT(MyPlugin(IEventService* eventService, ILoggerService* logger));
    ~MyPlugin() override;

    void Init() override;
    void Run() override;
    void Destroy() override;

    std::string GetName() const override;
    std::thread::id GetThreadId() const override;

private:
    IEventService* eventService;
    ILoggerService* logger;
    std::atomic<bool> running;
};

#endif // MYPLUGIN_H
