#include "PluginService.h"
#include <csignal>
#include <iostream>
#include <sstream>

// std::atomic<bool> PluginService::running = true;

PluginService::PluginService(IEventService* eventService, ILoggerService* logger)
    : eventService(eventService), logger(logger) {
        // std::signal(SIGINT, PluginService::SignalHandler);
}

void PluginService::RegisterPlugin(std::shared_ptr<IPlugin> plugin) {
    std::lock_guard<std::mutex> lock(initMutex);
    plugins.push_back(plugin);
    totalPlugins++;
    (*logger) << "[PluginService] Plugin registered: " << plugin->GetName() << std::endl;
}

void PluginService::InitPlugins() {
    (*logger) << "[PluginService] Initializing plugins..." << std::endl;

    for (auto& plugin : plugins) {
        (*logger) << "[PluginService] Initializing plugin: " << plugin->GetName() << std::endl;

        pluginThreads.emplace_back([this, plugin] {
            try {
                plugin->Init();
            } catch (const std::exception& e) {
                (*logger) << "[PluginService] Plugin Init() failed: " << e.what() << std::endl;
                return;
            } catch (...) {
                (*logger) << "[PluginService] Plugin Init() encountered an unknown error!" << std::endl;
                return;
            }

            {
                std::lock_guard<std::mutex> lock(initMutex);
                initializedPlugins++;
            }
            initCondition.notify_one();
        });
    }

    // wait until all plugins are initialized
    std::unique_lock<std::mutex> lock(initMutex);
    initCondition.wait(lock, [this] { return initializedPlugins.load() == totalPlugins.load(); });

    (*logger) << "[PluginService] All plugins initialized." << std::endl;
    StartPlugins();
}

void PluginService::StartPlugins() {
    (*logger) << "[PluginService] Starting plugins..." << std::endl;

    for (size_t i = 0; i < plugins.size(); ++i) {
        pluginThreads.emplace_back([plugin = plugins[i], this] {
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


// void PluginService::WaitForPlugins() {
//     std::lock_guard<std::mutex> lock(shutdownMutex);
//     if (shutdownCalled) return;  // already called

//     shutdownCalled = true;
//     (*logger) << "[PluginService] Waiting for plugins to finish..." << std::endl;
    
//     while (running) {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//     }

//     (*logger) << "[PluginService] Stopping plugins..." << std::endl;

//     for (auto& plugin : plugins) {
//         plugin->Destroy();
//     }

//     for (auto& thread : pluginThreads) {
//         if (thread.joinable()) {
//             thread.join();
//         }
//     }

//     (*logger) << "[PluginService] All plugins have been stopped." << std::endl;
// }

// void PluginService::SignalHandler(int signal) {
//     std::cout << "[PluginService] Signal " << signal << " received!" << std::endl;
//     if (signal == SIGINT) {
//         running = false;
//     }
// }

PluginService::~PluginService() {
    (*logger) << "[PluginService] Destructor called, need to wait for plugins to stop: " << (shutdownCalled == true ? "false" : "true") << std::endl;
    // WaitForPlugins();
}
