#ifndef IREPLICASERVICE_H
#define IREPLICASERVICE_H

#include "IReplica.h"
#include <memory>
#include <string>

class IReplicaService {
public:
    virtual ~IReplicaService() = default;

    virtual void RegisterReplica(std::shared_ptr<IReplica> replica) = 0;
    virtual void SendReplica(const IReplica& replica) = 0;
    virtual void ReceiveReplicas() = 0;
};

#endif // IREPLICASERVICE_H
