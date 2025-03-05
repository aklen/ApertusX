# Apertus - a C++ Plugin System with Dependency Injection

Apertus is a plugin-based system designed to initialize and run plugins in separate threads. It uses **Dependency Injection (DI)** with **Fruit**, a lightweight C++ dependency injection framework.


## Features
- 🏗️ **Dependency Injection** (via [Fruit](https://github.com/google/fruit))
- 🧩 **Plugin Registration**: Dynamically register plugins.
- 🚀 **Multithreaded Plugin Execution**: Each plugin runs in its own thread.
- ⏳ **Synchronized Initialization**: Ensures all plugins are initialized before execution.
- 🔄 **Event-Driven Execution**: Uses an `EventService` to handle events.
- ⚡ **Async Event Processing**: Plugins receive and handle events independently, even if their `Run()` method executes long-running tasks.
- 📜 **Logging System**: Thread-safe logging with a custom `LoggerService`.


## 🔧 Build and Run

### Prerequisites
- [C++17](https://en.cppreference.com/w/cpp/17) or later
- [CMake](https://cmake.org/)
- [Clang](https://clang.llvm.org/) (or [GCC](https://gcc.gnu.org/))
- [Fruit](https://github.com/google/fruit) (DI framework)

### Build Instructions
```sh
git clone https://github.com/your-repo/apertus.git
cd apertus
./build.sh
```

### Run

To run the application, execute:
```sh
cd build
./apertus
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

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch.
3. Commit your changes.
4. Submit a pull request.


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


## License

MIT License

Copyright (c) 2025 Akos Hamori

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
2. The Software may be used freely for personal and non-commercial projects.
3. Commercial use by companies is permitted **only if they agree to have their name and/or logo displayed in the "Used By" section of the official project website**. The project owner reserves the right to verify compliance.
4. The Software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose, and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages, or other liability, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the Software or the use or other dealings in the Software.

