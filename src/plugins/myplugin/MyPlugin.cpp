#include "MyPlugin.h"
#include <iostream>

MyPlugin::MyPlugin(IEventService* eventService, ILoggerService* logger)
    : Plugin(eventService, logger) {}

MyPlugin::~MyPlugin() {
    (*logger) << "[MyPlugin] Destructor called." << std::endl;
}

std::string MyPlugin::GetName() const {
    return "MyPlugin";
}

std::thread::id MyPlugin::GetThreadId() const {
    return std::this_thread::get_id();
}

void MyPlugin::Init() {
    Plugin::Init();  // call base class method to start event listener thread
    (*logger) << "[MyPlugin]::Init() Initialized." << std::endl;

    subscribe("OnStart", [this](const std::string& param) {
        (*logger) << "[MyPlugin]::Init() Handling OnStart event, param: " << param << std::endl;
    });

    subscribe("CustomEvent", [this](const std::string& param) {
        (*logger) << "[MyPlugin]::Init() Handling CustomEvent, param: " << param << std::endl;
    });
}

void MyPlugin::Run() {
    running = true;
    (*logger) << "[MyPlugin]::Run() Running on thread ID: " << GetThreadId() << std::endl;
    while (running) {
        eventService->Trigger("OnUpdate");
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work
    }
    (*logger) << "[MyPlugin]::Run() Stopped." << std::endl;
}

void MyPlugin::Destroy() {
    running = false;
    Plugin::Destroy();
}
