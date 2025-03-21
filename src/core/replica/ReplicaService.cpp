#include "ReplicaService.h"
#include <iostream>

ReplicaService::ReplicaService(IEventService* eventService, ILoggerService* logger)
    : eventService(eventService), logger(logger) {}

void ReplicaService::RegisterReplica(std::shared_ptr<IReplica> replica) {
    replicas[replica->GetUniqueId()] = std::move(replica);
}

void ReplicaService::SendReplica(const IReplica& replica) {
    if (!replica.IsSynchronized()) return;
    std::cout << "[ReplicaService] Sending replica: " << replica.GetName() << std::endl;
    // Küldési logika itt...
}

void ReplicaService::ReceiveReplicas() {
    std::cout << "[ReplicaService] Receiving replicas..." << std::endl;
    // Fogadási logika itt...
}
