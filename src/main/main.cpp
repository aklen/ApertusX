#include "di/DependencyInjection.h"
#include "interfaces/IEventService.h"
#include "interfaces/IPluginService.h"
#include "interfaces/IPlugin.h"
#include "interfaces/IReplicaService.h"
#include "core/replica/ReplicaFactory.h"

// std
#include <atomic>
#include <csignal>
#include <mutex>
#include <condition_variable>

// plugins
#include "myplugin/MyPlugin.h"
#include "gstreamer/GStreamerPlugin.h"

// 3rd party
#include <fruit/fruit.h>

std::atomic<bool> isRunning(true);
std::mutex shutdownMutex;
std::condition_variable shutdownCondition;

void SignalHandler(int signal) {
    std::cout << "[Main] Signal " << signal << " received! Stopping..." << std::endl;
    {
        std::lock_guard<std::mutex> lock(shutdownMutex);
        isRunning = false;
    }
    shutdownCondition.notify_one();
    // shutdownCondition.notify_all();
}

int main() {
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    // initialize DI container
    fruit::Injector<IEventService, ILoggerService, IConfigService, IPluginService, IReplicaService> injector(getApertusComponent);

    // load services from DI container
    auto eventService = injector.get<IEventService*>();
    auto pluginService = injector.get<IPluginService*>();
    auto loggerService = injector.get<ILoggerService*>();
    auto replicaService = injector.get<IReplicaService*>();

    // Start event processing
    eventService->Start();

    // subscribe to events
    {
        eventService->Subscribe("PlaybackStarted", [](const std::string& message) {
            std::cout << "[Event] Playback started: " << message << std::endl;
        });

        eventService->Subscribe("PlaybackStopped", [](const std::string& message) {
            std::cout << "[Event] Playback stopped: " << message << std::endl;
        });

        eventService->Subscribe("PlaybackFinished", [](const std::string& message) {
            std::cout << "[Event] Playback finished: " << message << std::endl;
        });

        eventService->Subscribe("PlaybackError", [](const std::string& error) {
            std::cerr << "[Event] Playback error: " << error << std::endl;
        });

        eventService->Subscribe("PlaybackStateChanged", [](const std::string& stateChange) {
            std::cout << "[Event] Playback state change:" << stateChange << std::endl;
        });

        eventService->Subscribe("OnUpdate", [](const std::string&) {
            std::cout << "[Event] OnUpdate event catched!" << std::endl;
        });
    }

    (*loggerService) << "[Main] Apertus started!" << std::endl;

    // register and start plugins
    auto myPlugin = std::make_shared<MyPlugin>(eventService, loggerService);
    pluginService->RegisterPlugin(myPlugin);

    // auto gstreamerPlugin = std::make_shared<GStreamerPlugin>(eventService, loggerService);
    // pluginService->RegisterPlugin(gstreamerPlugin);

    // initialize and start plugins
    pluginService->InitPlugins();

    (*loggerService) << "[Main] Plugins initialized and started." << std::endl;

    // test replica
    auto myReplica = ReplicaFactory::CreateReplica("Player1");
    replicaService->RegisterReplica(myReplica);
    replicaService->SendReplica(*myReplica);

    // trigger start event
    eventService->Trigger("OnStart");
    eventService->Trigger("PlayAudio", "file:///Users/aklen/Music/Ableton/Projects/647 Project/export/647.mp3");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    eventService->Trigger("PauseAudio");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    eventService->Trigger("ResumeAudio");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    eventService->Trigger("StopAudio");

    // Wait for termination signal using condition_variable
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        (*loggerService) << "[Main] Running..." << std::endl;
        eventService->Trigger("CustomEvent");
    }
    // {
    //     std::cout << "[Main] Waiting for shutdown signal..." << std::endl;
    //     std::unique_lock<std::mutex> lock(shutdownMutex);
    //     shutdownCondition.wait(lock, [] { return !isRunning; });
    // }

    (*loggerService) << "[Main] Stopping plugins in correct order..." << std::endl;

    pluginService->StopPlugins();

    (*loggerService) << "[Main] Stopping EventService..." << std::endl;
    eventService->Stop();  // Stop the event loop
    (*loggerService) << "[Main] EventService stopped." << std::endl;

    (*loggerService) << "[Main] Destroying PluginService..." << std::endl;
    pluginService = nullptr;

    (*loggerService) << "[Main] Destroying EventService..." << std::endl;
    eventService = nullptr;

    (*loggerService) << "[Main] Destroying LoggerService..." << std::endl;
    loggerService = nullptr;

    std::cout << "[Main] Shutdown complete." << std::endl;
    return 0;
}
