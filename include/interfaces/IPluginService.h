#ifndef IPLUGINSERVICE_H
#define IPLUGINSERVICE_H

#include <memory>
#include <vector>
#include "IPlugin.h"

class IPluginService {
public:
    virtual ~IPluginService() = default;
    virtual void RegisterPlugin(std::shared_ptr<IPlugin> plugin) = 0;
    virtual void InitPlugins() = 0;
    virtual void StartPlugins() = 0;
    virtual void StopPlugins() = 0;
};

#endif // IPLUGINSERVICE_H
