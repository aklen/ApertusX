#pragma once

#include <string>
#include <functional>

class IReplicaRegistrar {
public:
    virtual ~IReplicaRegistrar() = default;

    // Registers a replica type with the given type name
        template<typename T>
        void Register(const std::string& type) {
            RegisterImpl(type);
        }
    
    protected:
        virtual void RegisterImpl(const std::string& type) = 0;

    // Registers all pending replicas
    virtual void RegisterAllReplicas() = 0;

    // Adds a pending registration for a replica type
        template<typename T>
        void AddPendingRegistration(const std::string& type) {
            AddPendingRegistrationImpl(type);
        }
    
    protected:
        virtual void AddPendingRegistrationImpl(const std::string& type) = 0;
};
