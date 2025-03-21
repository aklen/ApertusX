#pragma once

#include "IReplica.h"
#include <memory>
#include <string>

class IReplicaService {
public:
    virtual ~IReplicaService() = default;

    // Registers a replica object for synchronization
    virtual void RegisterReplica(std::shared_ptr<IReplica> replica) = 0;

    // Sends a replica object over the network
    virtual void SendReplica(const IReplica& replica) = 0;

    // Starts receiving replicas from the network
    virtual void ReceiveReplicas() = 0;
};
