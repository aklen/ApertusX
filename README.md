# ApertusX - A Modular C++ Framework for Plugin-Based and Synchronized Systems

ApertusX is a **plugin-based system** designed to initialize and run plugins in separate threads while maintaining a **single source of truth** across multiple instances. It integrates **Dependency Injection (DI)**, and introduces **ReplicaService** for real-time entity synchronization across networked devices.


## Features
- 🏗️ **Dependency Injection** (via [Fruit](https://github.com/google/fruit))
- 🧩 **Plugin Registration**: Dynamically register plugins.
- 🚀 **Multithreaded Plugin Execution**: Each plugin runs in its own thread.
- ⏳ **Synchronized Initialization**: Ensures all plugins are initialized before execution.
- 🔄 **Event-Driven Execution**: Uses an `EventService` to handle events.
- ⚡ **Async Event Processing**: Plugins receive and handle events independently, even if their `Run()` method executes long-running tasks.
- 📜 **Logging System**: Thread-safe logging with a custom `LoggerService`.
- 🌐 **ReplicaService for State Synchronization**: Maintains a consistent and synchronized state of entities across multiple instances.
- 🎵 **Efficient Audio Stream Synchronization**: Instead of transmitting raw audio streams, the system synchronizes only `AudioEntity` state changes, ensuring all instances play the same audio in sync.


## Origin
ApertusX is based on [**ApertusVR**](https://github.com/MTASZTAKI/ApertusVR), a research and development project from the **Hungarian Academy of Sciences (MTA SZTAKI)**. ApertusVR was an **open-source extended reality software library** designed for industrial cyber-physical systems, focusing on **distributed multi-user virtual reality applications**. It introduced a **distributed plugin mechanism**, enabling **real-time digital twin synchronization**, **robotics integration**, and **cross-platform extended reality (XR) solutions**.

ApertusX adopts and extends the **distributed synchronization architecture** of ApertusVR to provide a **replicated object model** for **general entity synchronization**, initially focusing on **audio stream synchronization** across multiple instances.


## Architecture Overview

ApertusX is built around several core services that manage plugin execution, event handling, and distributed object synchronization.

### **LoggerService**
Provides a centralized logging mechanism to capture and store logs from various system components. It ensures thread-safe logging and supports different log levels.

### **ConfigService**
Manages configuration settings and provides a unified way for plugins and services to access runtime settings.

### **EventService**
Implements an event-driven architecture where plugins can subscribe to and trigger events. This enables seamless inter-plugin communication.

### **PluginService**
Handles the lifecycle of plugins, including registration, initialization, and execution. It ensures that all plugins are initialized before any execution begins.

### **ReplicaService**
Provides a distributed object synchronization system. It maintains a **single source of truth** for shared objects across multiple instances. Changes to an object in one instance are automatically synchronized across all connected instances.


## 🔧 Build and Run

### Prerequisites
- [C++17](https://en.cppreference.com/w/cpp/17) or later
- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/) (or [GCC](https://gcc.gnu.org/))
- [Fruit](https://github.com/google/fruit) (DI framework)

### Build Instructions
```sh
git clone https://github.com/aklen/apertusx.git
cd apertusx
./build.sh
```

### Run

To run the application, execute:
```sh
cd build
./apertusx
```


## Plugin Development

1. **Implement the `IPlugin` Interface**
```cpp
#include "interfaces/IPlugin.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <fruit/fruit.h>

class CustomPlugin : public IPlugin {
public:
    INJECT(CustomPlugin(IEventService* eventService, ILoggerService* logger));

    void Init() override {
        (*logger) << "[CustomPlugin] Plugin initialized!" << std::endl;
    }

    void Run() override {
        (*logger) << "[CustomPlugin] Running on thread ID: " << std::this_thread::get_id() << std::endl;
    }

    void Stop() override {
        (*logger) << "[CustomPlugin] Stopping..." << std::endl;
    }
};
```

2. **Register the Plugin**
```cpp
auto customPlugin = std::make_shared<CustomPlugin>(
    eventService, // inject dependency via variable
    injector.get<ILoggerService*>() // or inject dependency by using injector.get() with an interface
);
pluginService->RegisterPlugin(customPlugin);
```

## Replica-Based Data Synchronization

### ReplicaService as the Single Source of Truth
ReplicaService ensures all instances maintain a consistent view of synchronized entities. Each instance can query and modify entities, and any change is automatically replicated across the network.

### Replication Mechanism for Efficient Audio Data Synchronization  
- **Entity Serialization**: Each `AudioEntity` can be converted into a byte stream for efficient transmission.
- **Delta Synchronization**: Only changes (deltas) are synchronized, reducing bandwidth usage.
- **Flexible Network Topologies**: Supports both **peer-to-peer (P2P) and star topology** replication using RakNet, SLikeNet, or ENet.

### Event-Driven Audio Management
- The **EventManager** listens for updates to `AudioEntity` and triggers appropriate responses.
- GStreamer playback is **automatically updated** based on the latest synchronized state, ensuring each instance remains in sync.

### Workflow
1. An instance creates an `AudioEntity` and starts streaming.
2. The streaming data modifies the `AudioEntity`, triggering an update in the **ReplicaService**.
3. The updated state is **automatically synchronized** with all connected instances.
4. Other instances receive the updated `AudioEntity` state and update their local **GStreamer playback** accordingly.
5. If an instance modifies an entity (e.g., pausing playback), the change is **propagated back** to the ReplicaService, ensuring global synchronization.

### Advantages of this Approach
- **Scalable and Modular**: The system is independent of specific streaming sources (Spotify, local files, etc.), making it highly adaptable.  
-  **Efficient Synchronization**: Instead of broadcasting raw audio streams, only necessary state changes are synchronized.  
-  **Decoupled Architecture**: GStreamer follows the state of `AudioEntity`, eliminating direct network management.  
-  **Optimized for Latency**: Using delta updates ensures low-latency synchronization across multiple devices.  
-  **Flexible Transport Layer**: Supports multiple networking backends (RakNet, SLikeNet, ENet).  


## Development Recommendations

### Graceful Shutdown Handling

When implementing a graceful shutdown mechanism in your application, it is recommended to use `std::condition_variable` for waiting on shutdown signals. This approach is more efficient and responsive compared to using a busy-wait loop. Below is an example of how to implement this in your `main.cpp` file:

```cpp
#include <atomic>
#include <csignal>
#include <mutex>
#include <condition_variable>
#include <iostream>

// Global variables for shutdown handling
std::atomic<bool> isRunning(true);
std::mutex shutdownMutex;
std::condition_variable shutdownCondition;

// Signal handler function
void SignalHandler(int signal) {
    std::cout << "[Main] Signal " << signal << " received! Stopping..." << std::endl;
    {
        std::lock_guard<std::mutex> lock(shutdownMutex);
        isRunning = false;
    }
    shutdownCondition.notify_one();
}

int main() {
    // Register signal handlers
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    // Initialize and start services and plugins
    // ...

    // Wait for termination signal using condition_variable
    {
        std::cout << "[Main] Waiting for shutdown signal..." << std::endl;
        std::unique_lock<std::mutex> lock(shutdownMutex);
        shutdownCondition.wait(lock, [] { return !isRunning; });
    }

    // Stop plugins and services in the correct order
    (*loggerService) << "[Main] Stopping plugins in correct order..." << std::endl;
    pluginService->StopPlugins();

    eventService->Stop();  // Stop the event loop

    (*loggerService) << "[Main] Destroying PluginService..." << std::endl;
    pluginService = nullptr;

    (*loggerService) << "[Main] Destroying EventService..." << std::endl;
    eventService = nullptr;

    (*loggerService) << "[Main] Destroying LoggerService..." << std::endl;
    loggerService = nullptr;

    std::cout << "[Main] Shutdown complete." << std::endl;
    return 0;
}
```

### Explanation

- **Signal Handling**: The `SignalHandler` function sets the `isRunning` flag to `false` and notifies the condition variable when a termination signal (e.g., SIGINT or SIGTERM) is received.
- **Condition Variable**: The `std::condition_variable` is used to wait for the shutdown signal efficiently. This avoids busy-waiting and reduces CPU usage.
- **Orderly Shutdown**: The plugins and services are stopped in the correct order, ensuring that resources are released properly and the application shuts down gracefully.

By following this approach, you can ensure that your application handles shutdown signals efficiently and shuts down gracefully.


## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch.
3. Commit your changes.
4. Submit a pull request.


## License

MIT License

Copyright (c) 2025 Akos Hamori

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
2. The Software may be used freely for personal and non-commercial projects.
3. Commercial use by companies is permitted **only if they agree to have their name and/or logo displayed in the "Used By" section of the official project website**. The project owner reserves the right to verify compliance.
4. The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the Software or the use or other dealings in the Software.
