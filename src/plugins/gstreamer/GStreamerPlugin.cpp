#include "GStreamerPlugin.h"
#include <iostream>
#include <string>
#include <regex>
#include <iomanip>
#include <gst/gst.h>
#include <curl/curl.h>
#include "UrlUtils.h"

GStreamerPlugin::GStreamerPlugin(IEventService* eventService, ILoggerService* logger) 
    : eventService(eventService), logger(logger), pipeline(nullptr), running(false) {
    gst_init(nullptr, nullptr);
}

std::string GStreamerPlugin::GetName() const {
    return "GStreamerPlugin";
}

std::thread::id GStreamerPlugin::GetThreadId() const {
    return std::this_thread::get_id();
}

void GStreamerPlugin::Init() {
    (*logger) << "[GStreamerPlugin]::Init() Initialized." << std::endl;

    eventService->Subscribe("PlayAudio", [this](const std::string& uri) {
        (*this->logger) << "[GStreamerPlugin]::Init() PlayAudio event received: " << uri << std::endl;
        this->Play(uri);
    });

    eventService->Subscribe("PauseAudio", [this](const std::string&) {
        (*this->logger) << "[GStreamerPlugin]::Init() PauseAudio event received." << std::endl;
        this->Pause();
    });

    eventService->Subscribe("ResumeAudio", [this](const std::string&) {
        (*this->logger) << "[GStreamerPlugin]::Init() ResumeAudio event received." << std::endl;
        this->Resume();
    });

    eventService->Subscribe("StopAudio", [this](const std::string&) {
        (*this->logger) << "[GStreamerPlugin]::Init() StopAudio event received." << std::endl;
        this->Stop();
    });
}

void GStreamerPlugin::Run() {
    (*logger) << "[GStreamerPlugin] Running..." << std::endl;
}

void GStreamerPlugin::Play(const std::string& uri) {
    std::string cleanedUri = UrlUtils::ToFileUri(uri);
    if (cleanedUri.empty()) {
        (*logger) << "[GStreamerPlugin] Invalid file path: " << cleanedUri << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin] Original URI: " << uri << std::endl;
    (*logger) << "[GStreamerPlugin] Cleaned URI: " << cleanedUri << std::endl;

    pipeline = gst_parse_launch(("playbin uri=" + cleanedUri).c_str(), nullptr);
    if (!pipeline) {
        (*logger) << "[GStreamerPlugin] Failed to create pipeline!" << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin] Setting up GStreamer bus..." << std::endl;
    GstBus* bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, (GstBusFunc)OnBusMessage, this);
    gst_object_unref(bus);
    (*logger) << "[GStreamerPlugin] Bus watch added." << std::endl;

    // start playback in a separate thread
    std::thread([this] {
        (*logger) << "[GStreamerPlugin] Changing state to PLAYING..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        running = true;

        eventService->Trigger("PlaybackStarted", "Playback started");
        (*logger) << "[GStreamerPlugin] Playback started." << std::endl;

        // gstThread = std::thread(&GStreamerPlugin::GStreamerMainLoop, this);
        // (*logger) << "[GStreamerPlugin] GStreamer main loop thread started." << std::endl;
    }).detach();
}

void GStreamerPlugin::Stop() {
    if (!running) {
        (*logger) << "[GStreamerPlugin] Stop called, but playback is already stopped." << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin] Stopping playback..." << std::endl;
    running = false; // Mark playback as stopped

    if (pipeline) {
        (*logger) << "[GStreamerPlugin] Changing state to NULL..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_NULL); // Stop the pipeline
        (*logger) << "[GStreamerPlugin] Pipeline state changed to NULL." << std::endl;

        gst_object_unref(pipeline); // Unref the pipeline

        pipeline = nullptr; // Set the pipeline to null, prevent double-free
        (*logger) << "[GStreamerPlugin] Pipeline stopped and freed." << std::endl;
    }

    eventService->Trigger("PlaybackStopped", "Playback stopped");

    // (*logger) << "[GStreamerPlugin] Waiting for GStreamer thread to join..." << std::endl;
    // if (gstThread.joinable()) {
    //     (*logger) << "[GStreamerPlugin] Joining playback thread..." << std::endl;
    //     gstThread.join();
    // }
    // (*logger) << "[GStreamerPlugin] GStreamer thread joined." << std::endl;

    (*logger) << "[GStreamerPlugin] Playback stopped completely." << std::endl;
}

void GStreamerPlugin::Destroy() {
    (*logger) << "[GStreamerPlugin] Destroying..." << std::endl;
    Stop();
    gst_deinit();
}

void GStreamerPlugin::Pause() {
    if (pipeline && running) {
        (*logger) << "[GStreamerPlugin] Pausing playback..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    }
}

void GStreamerPlugin::Resume() {
    if (pipeline && running) {
        (*logger) << "[GStreamerPlugin] Resuming playback..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}

// --- Private methods ---

void GStreamerPlugin::GStreamerMainLoop() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GStreamerPlugin::OnBusMessage(GstBus* bus, GstMessage* msg, gpointer data) {
    GStreamerPlugin* plugin = static_cast<GStreamerPlugin*>(data);

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            (*plugin->logger) << "[GStreamerPlugin]::OnBusMessage End of stream reached!" << std::endl;
            plugin->Stop();
            // plugin->eventService->Trigger("PlaybackFinished", "Playback completed successfully");
            break;
        case GST_MESSAGE_ERROR: {
            GError* err;
            gchar* debug;
            gst_message_parse_error(msg, &err, &debug);
            (*plugin->logger) << "[GStreamerPlugin]::OnBusMessage Error: " << err->message << std::endl;
            // (*plugin->eventService).Trigger("PlaybackError", err->message);
            g_error_free(err);
            g_free(debug);
            plugin->Stop();
            break;
        }
        // case GST_MESSAGE_STATE_CHANGED: {
        //     if (GST_MESSAGE_SRC(msg) == GST_OBJECT(plugin->pipeline)) { 
        //         // Get old and new states
        //         GstState old_state, new_state, pending;
        //         gst_message_parse_state_changed(msg, &old_state, &new_state, &pending);

        //         // Log state change
        //         (*plugin->logger) << "[GStreamerPlugin] State changed from "
        //                           << gst_element_state_get_name(old_state) << " to "
        //                           << gst_element_state_get_name(new_state) << std::endl;

        //         // Optionally, trigger an event
        //         std::string stateChangeEvent = "StateChanged_" + std::string(gst_element_state_get_name(new_state));
        //         std::string stateTransition = std::string(gst_element_state_get_name(old_state)) +
        //                                       " → " + std::string(gst_element_state_get_name(new_state));
        //         plugin->eventService->Trigger("PlaybackStateChanged", stateTransition);
        //     }
        //     break;
        // }
        default:
            break;
    }
}
