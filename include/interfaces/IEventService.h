#ifndef IEVENTSERVICE_H
#define IEVENTSERVICE_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

/**
 * @class IEventService
 * @brief Interface for an event service that allows subscribing to and triggering events.
 */
class IEventService {
public:

    /**
     * @brief Virtual destructor for the IEventService interface.
     */
    virtual ~IEventService() = default;

    /**
     * @typedef EventCallback
     * @brief Type definition for the event callback function.
     * @param std::string The event parameter.
     */
    using EventCallback = std::function<void(const std::string&)>;

    /**
     * @brief Subscribes to an event with a callback function.
     * @param eventName The name of the event to subscribe to.
     * @param callback The callback function to be called when the event is triggered.
     */
    virtual void Subscribe(const std::string& eventName, EventCallback callback) = 0;

    /**
     * @brief Unsubscribes from an event with a callback function.
     * @param eventName The name of the event to unsubscribe from.
     * @param callback The callback function to be removed from the event subscribers.
     */
    virtual void Unsubscribe(const std::string& eventName, EventCallback callback) = 0;

    /**
     * @brief Triggers an event with an optional parameter.
     * @param eventName The name of the event to trigger.
     * @param param The optional parameter to pass to the event callback.
     */
    virtual void Trigger(const std::string& eventName, const std::string& param = "") = 0;

    /**
     * @brief Starts the event service.
     */
    virtual void Start() = 0;

    /**
     * @brief Stops the event service.
     * @details This method stops the event service, ensuring that no further events are processed.
     * Any ongoing event processing should be completed or terminated gracefully.
     */
    virtual void Stop() = 0;
};

#endif // IEVENTSERVICE_H
