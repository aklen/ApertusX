#include "ReplicaFactory.h"
#include "replicas/AudioReplica.h"

std::shared_ptr<IReplica> ReplicaFactory::CreateReplica(const std::string& type) {
    if (type == "AudioReplica") {
        return std::make_shared<AudioReplica>();
    }
    return nullptr;
}
