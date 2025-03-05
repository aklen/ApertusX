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
    while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void GStreamerPlugin::Play(const std::string& uri) {
    std::string cleanedUri = UrlUtils::ToFileUri(uri);
    if (cleanedUri.empty()) {
        (*logger) << "[GStreamerPlugin]::Play() Invalid file path: " << cleanedUri << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin]::Play() Original URI: " << uri << std::endl;
    (*logger) << "[GStreamerPlugin]::Play() Cleaned URI: " << cleanedUri << std::endl;

    pipeline = gst_parse_launch(("playbin uri=" + cleanedUri).c_str(), nullptr);
    if (!pipeline) {
        (*logger) << "[GStreamerPlugin]::Play() Failed to create pipeline!" << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin]::Play() Setting up GStreamer bus..." << std::endl;
    GstBus* bus = gst_element_get_bus(pipeline);
    gst_bus_add_watch(bus, (GstBusFunc)OnBusMessage, this);
    gst_object_unref(bus);
    (*logger) << "[GStreamerPlugin]::Play() Bus watch added." << std::endl;

    // start playback in a separate thread
    std::thread([this] {
        (*logger) << "[GStreamerPlugin]::Play() Changing state to PLAYING..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        running = true;

        eventService->Trigger("PlaybackStarted", "Playback started");
        (*logger) << "[GStreamerPlugin]::Play() Playback started." << std::endl;

        // gstThread = std::thread(&GStreamerPlugin::GStreamerMainLoop, this);
        // (*logger) << "[GStreamerPlugin]::Play() GStreamer main loop thread started." << std::endl;
    }).detach();
}

void GStreamerPlugin::Stop(bool force) {
    if (!running && !force) {
        (*logger) << "[GStreamerPlugin]::Stop() Stop called, but playback is already stopped." << std::endl;
        return;
    }

    (*logger) << "[GStreamerPlugin]::Stop() Stopping playback..." << std::endl;
    running = false; // Mark playback as stopped

    if (pipeline) {
        (*logger) << "[GStreamerPlugin]::Stop() Changing state to NULL..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_NULL); // Stop the pipeline
        (*logger) << "[GStreamerPlugin]::Stop() Pipeline state changed to NULL." << std::endl;


        (*logger) << "[GStreamerPlugin]::Stop() Removing bus watch..." << std::endl;
        GstBus* bus = gst_element_get_bus(pipeline);
        gst_bus_remove_watch(bus);
        gst_object_unref(bus);
        (*logger) << "[GStreamerPlugin]::Stop() Bus watch removed." << std::endl;

        // Send a final message to the pipeline
        gst_element_post_message(pipeline, gst_message_new_application(GST_OBJECT(pipeline), gst_structure_new_empty("shutdown")));

        (*logger) << "[GStreamerPlugin]::Stop() Unref'ing pipeline..." << std::endl;
        gst_object_unref(pipeline); // Unref the pipeline
        pipeline = nullptr; // Set the pipeline to null, prevent double-free
        (*logger) << "[GStreamerPlugin]::Stop() Pipeline stopped and freed." << std::endl;


        eventService->Trigger("PlaybackStopped", "Playback stopped");
    }

    (*logger) << "[GStreamerPlugin]::Stop() Checking if GStreamer thread should join..." << std::endl;
    if (gstThread.joinable()) {
        (*logger) << "[GStreamerPlugin]::Stop() Joining playback thread..." << std::endl;
        gstThread.join();
        (*logger) << "[GStreamerPlugin]::Stop() Playback thread joined." << std::endl;
    } else {
        (*logger) << "[GStreamerPlugin]::Stop() No thread to join." << std::endl;
    }
    (*logger) << "[GStreamerPlugin]::Stop() Playback stopped." << std::endl;
}

void GStreamerPlugin::Destroy() {
    (*logger) << "[GStreamerPlugin]::Destroy() Destroying..." << std::endl;
    Stop(true);
    (*logger) << "[GStreamerPlugin]::Destroy() Stopped." << std::endl;
    gst_deinit();
    (*logger) << "[GStreamerPlugin]::Destroy() Deinitialized." << std::endl;
}

GStreamerPlugin::~GStreamerPlugin() {
    (*logger) << "[GStreamerPlugin]::~GStreamerPlugin() Destructor called." << std::endl;
    Destroy();
    (*logger) << "[GStreamerPlugin]::~GStreamerPlugin() Destroyed." << std::endl;
}

void GStreamerPlugin::Pause() {
    if (pipeline && running) {
        (*logger) << "[GStreamerPlugin]::Pause() Pausing playback..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
    }
}

void GStreamerPlugin::Resume() {
    if (pipeline && running) {
        (*logger) << "[GStreamerPlugin]::Resume() Resuming playback..." << std::endl;
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
}

// --- Private methods ---

void GStreamerPlugin::GStreamerMainLoop() {
    // while (running) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* msg;

    while (running) {
        (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Waiting for messages..." << std::endl;
        msg = gst_bus_timed_pop_filtered(bus, GST_SECOND, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_APPLICATION));

        if (msg != nullptr) {
            (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Message received: " << GST_MESSAGE_TYPE_NAME(msg) << std::endl;

            // 🔥 Ha a shutdown üzenetet kapjuk, kilépünk a loopból!
            if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_APPLICATION) {
                (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Shutdown message received, exiting..." << std::endl;
                gst_message_unref(msg);
                break;
            }

            OnBusMessage(bus, msg, this);
            gst_message_unref(msg);
        } else {
            (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Timeout, checking running flag..." << std::endl;
        }

        if (!running) {
            (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Exiting loop..." << std::endl;
            break;
        }
    }

    gst_object_unref(bus);
    (*logger) << "[GStreamerPlugin]::GStreamerMainLoop() Exiting..." << std::endl;
    
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
