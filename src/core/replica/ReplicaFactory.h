#ifndef REPLICAFACTORY_H
#define REPLICAFACTORY_H

#include "interfaces/IReplica.h"
#include <memory>

class ReplicaFactory {
public:
    static std::shared_ptr<IReplica> CreateReplica(const std::string& type);
};

#endif // REPLICAFACTORY_H
