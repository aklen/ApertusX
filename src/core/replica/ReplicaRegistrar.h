#pragma once
#include "ReplicaFactory.h"
#include "IReplicaRegistrar.h"
#include <vector>
#include <fruit/fruit.h>

class ReplicaRegistrar : public IReplicaRegistrar {
public:
    INJECT(ReplicaRegistrar(IReplicaFactory* factory)) : factory(factory) {}

    template<typename T>
    void Register(const std::string& type) {
        factory->RegisterReplicaType(type, 
            [](const std::string& name, bool sync, const std::string& owner, bool isHost) {
                return std::make_shared<T>(name, sync, owner, isHost);
            });
    }

    void RegisterAllReplicas() {
        for (const auto& regFunc : pendingRegistrations) {
            regFunc();
        }
    }

    template<typename T>
    void AddPendingRegistration(const std::string& type) {
        pendingRegistrations.push_back([this, type]() {
            Register<T>(type);
        });
    }

private:
    IReplicaFactory* factory;
    std::vector<std::function<void()>> pendingRegistrations;
};
