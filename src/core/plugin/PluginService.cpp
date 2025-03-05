#include "PluginService.h"
#include <iostream>

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

                (*logger) << "[PluginService] Starting event listener for: " << plugin->GetName() << std::endl;
                StartPluginEventListener(plugin);

            } catch (const std::exception& e) {
                (*logger) << "[PluginService] Plugin Init() failed: " << e.what() << std::endl;
            } catch (...) {
                (*logger) << "[PluginService] Plugin Init() encountered an unknown error!" << std::endl;
            }
        });
    }

    // Wait until all plugins are initialized
    std::unique_lock<std::mutex> lock(initMutex);
    initCondition.wait(lock, [this] { return initializedPlugins.load() == totalPlugins.load(); });

    (*logger) << "[PluginService] All plugins initialized, starting Run()..." << std::endl;

    // Start each plugin on a separate thread
    for (auto& plugin : plugins) {
        pluginThreads.emplace_back([plugin, this] {
            try {
                (*logger) << "[PluginService] Running plugin: " << plugin->GetName() << std::endl;
                plugin->Run();
            } catch (const std::exception& e) {
                (*logger) << "[PluginService] Plugin Run() failed: " << e.what() << std::endl;
            } catch (...) {
                (*logger) << "[PluginService] Plugin Run() encountered an unknown error!" << std::endl;
            }
        });
    }
}

void PluginService::StartPluginEventListener(std::shared_ptr<IPlugin> plugin) {
    eventThreads.emplace_back([plugin, this] {
        while (!shutdownCalled) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Polling for events
        }
    });
}

void PluginService::StopPlugins() {
    std::lock_guard<std::mutex> lock(initMutex);
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

    for (auto& thread : eventThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    (*logger) << "[PluginService] All plugins have been stopped." << std::endl;
}

PluginService::~PluginService() {
    StopPlugins();
}
