#include "MyPlugin.h"
#include <iostream>

MyPlugin::MyPlugin(IEventService* eventService, ILoggerService* logger)
    : eventService(eventService), logger(logger), running(false) {}

std::string MyPlugin::GetName() const {
    return "MyPlugin";
}

std::thread::id MyPlugin::GetThreadId() const {
    return std::this_thread::get_id();
}

void MyPlugin::Init() {
    (*logger) << "[MyPlugin] Plugin loaded!" << std::endl;

    eventService->Subscribe("OnStart", [this](const std::string& param) {
        std::cout << "[MyPlugin] OnStart event catched!" << std::endl;
    });
}

void MyPlugin::Run() {
    running = true;

    (*logger) << "[MyPlugin] Running on thread ID: " << GetThreadId() << std::endl;

    while (running) {
        eventService->Trigger("OnUpdate");
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work
    }

    (*logger) << "[MyPlugin] Stopped." << std::endl;
}

void MyPlugin::Destroy() {
    running = false;
}
