#pragma once

#include <string>
#include <functional>
#include <memory>
#include "IReplica.h"

class IReplicaFactory {
public:
    virtual ~IReplicaFactory() = default;

    using CreateFunc = std::function<std::shared_ptr<IReplica>(const std::string&, bool, const std::string&, bool)>;

    // Registers a replica type with a creation function
    virtual void RegisterReplicaType(const std::string& type, CreateFunc func) = 0;

    // Creates a replica of the specified type
    virtual std::shared_ptr<IReplica> CreateReplica(const std::string& type, const std::string& name, bool sync, const std::string& owner, bool isHost) = 0;
};
