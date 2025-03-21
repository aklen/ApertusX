#ifndef REPLICASERVICE_H
#define REPLICASERVICE_H

#include "interfaces/IReplicaService.h"
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include <unordered_map>
#include <fruit/fruit.h>

class ReplicaService : public IReplicaService {
public:
    INJECT(ReplicaService(IEventService* eventService, ILoggerService* logger));
    void RegisterReplica(std::shared_ptr<IReplica> replica) override;
    void SendReplica(const IReplica& replica) override;
    void ReceiveReplicas() override;

private:
    IEventService* eventService;
    ILoggerService* logger;
    std::unordered_map<std::string, std::shared_ptr<IReplica>> replicas;
};

#endif // REPLICASERVICE_H
