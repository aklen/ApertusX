#ifndef GSTREAMERPLUGIN_H
#define GSTREAMERPLUGIN_H

#include "interfaces/IPlugin.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <string>
#include <thread>
#include <atomic>
#include <fruit/fruit.h>
#include <gst/gst.h>

class GStreamerPlugin : public IPlugin {
public:
    INJECT(GStreamerPlugin(IEventService* eventService, ILoggerService* logger));
    ~GStreamerPlugin() override;

    void Init() override;
    void Run() override;
    void Destroy() override;
    
    std::string GetName() const override;
    std::thread::id GetThreadId() const override;

    void Play(const std::string& uri);  // Play a file or URL
    void Stop(bool force = false);  // Stop the current playback
    void Pause();  // Pause the current playback
    void Resume();  // Resume the current playback

private:
    IEventService* eventService;
    ILoggerService* logger;
    GstElement* pipeline;
    std::atomic<bool> running;
    std::thread gstThread;

    static void OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data);
    void GStreamerMainLoop();
};

#endif // GSTREAMERPLUGIN_H
