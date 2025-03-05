#include "PluginService.h"
#include <csignal>
#include <iostream>
#include <sstream>

PluginService::PluginService(IEventService* eventService, ILoggerService* logger)
    : eventService(eventService), logger(logger) {}

void PluginService::RegisterPlugin(std::shared_ptr<IPlugin> plugin) {
    std::lock_guard<std::mutex> lock(initMutex);
    plugins.push_back(plugin);
    totalPlugins++;
    (*logger) << "[PluginService] Plugin registered: " << plugin->GetName() << std::endl;
}

void PluginService::InitPlugins() {
    (*logger) << "[PluginService] Initializing plugins..." << std::endl;

    for (auto& plugin : plugins) {
        pluginThreads.emplace_back([this, plugin] {
            try {
                (*logger) << "[PluginService] Initializing plugin: " << plugin->GetName() << std::endl;
                plugin->Init();
                {
                    std::lock_guard<std::mutex> lock(initMutex);
                    initializedPlugins++;
                }
                initCondition.notify_one();

                // Wait until all plugins are initialized
                std::unique_lock<std::mutex> lock(initMutex);
                initCondition.wait(lock, [this] { return initializedPlugins.load() == totalPlugins.load(); });

                (*logger) << "[PluginService] Running plugin: " << plugin->GetName() << std::endl;
                plugin->Run();
            } catch (const std::exception& e) {
                (*logger) << "[PluginService] Plugin Init() or Run() failed: " << e.what() << std::endl;
            } catch (...) {
                (*logger) << "[PluginService] Plugin Init() or Run() encountered an unknown error!" << std::endl;
            }
        });
    }

    // Wait until all plugins are initialized
    std::unique_lock<std::mutex> lock(initMutex);
    initCondition.wait(lock, [this] { return initializedPlugins.load() == totalPlugins.load(); });

    (*logger) << "[PluginService] All plugins initialized." << std::endl;
}

void PluginService::StartPlugins() {
    (*logger) << "[PluginService] Starting plugins..." << std::endl;
    // No need to start plugins here as they are already started in InitPlugins
}

void PluginService::StopPlugins() {
    std::lock_guard<std::mutex> lock(shutdownMutex);
    (*logger) << "[PluginService] StopPlugins()" << std::endl;
    if (shutdownCalled) return; 

    shutdownCalled = true;
    (*logger) << "[PluginService] Stopping plugins..." << std::endl;

    for (auto& plugin : plugins) {
        plugin->Destroy();
    }

    for (auto& thread : pluginThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    (*logger) << "[PluginService] All plugins have been stopped." << std::endl;
}

PluginService::~PluginService() {
    (*logger) << "[PluginService] Destructor called, need to wait for plugins to stop: " << (shutdownCalled == true ? "false" : "true") << std::endl;
    StopPlugins();
}
