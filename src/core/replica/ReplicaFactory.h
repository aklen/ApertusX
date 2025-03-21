#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "Replica.h"
#include "IReplicaFactory.h"
#include <fruit/fruit.h>

class ReplicaFactory : public IReplicaFactory {
public:
    INJECT(ReplicaFactory()) {}

    using CreateFunc = std::function<std::shared_ptr<IReplica>(const std::string&, bool, const std::string&, bool)>;

    void RegisterReplicaType(const std::string& type, CreateFunc func) {
        replicaRegistry[type] = func;
    }

    std::shared_ptr<IReplica> CreateReplica(const std::string& type, const std::string& name, bool sync, const std::string& owner, bool isHost) {
        if (replicaRegistry.find(type) != replicaRegistry.end()) {
            return replicaRegistry[type](name, sync, owner, isHost);
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, CreateFunc> replicaRegistry;
};
