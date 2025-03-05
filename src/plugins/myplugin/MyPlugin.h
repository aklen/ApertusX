#ifndef MYPLUGIN_H
#define MYPLUGIN_H

#include "interfaces/IPlugin.h"
#include "core/plugin/Plugin.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <memory>
#include <atomic>
#include <fruit/fruit.h>

class MyPlugin : public Plugin {
public:
    INJECT(MyPlugin(IEventService* eventService, ILoggerService* logger));
    ~MyPlugin() override;

    void Init() override;
    void Run() override;
    void Destroy() override;

    std::string GetName() const override;
    std::thread::id GetThreadId() const override;

private:
};

#endif // MYPLUGIN_H
