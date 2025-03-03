#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <string>
#include <thread>

class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual void Init() = 0;
    virtual void Run() = 0;
    virtual void Destroy() = 0;

    virtual std::string GetName() const = 0;
    virtual std::thread::id GetThreadId() const = 0;
};

#endif // IPLUGIN_H
