#ifndef REPLICA_H
#define REPLICA_H

#include "interfaces/IReplica.h"
#include <string>

class Replica : public IReplica {
public:
    Replica(std::string name, bool sync, std::string owner, bool isHost);

    std::string GetName() const override;
    std::string GetUniqueId() const override;
    bool IsSynchronized() const override;
    std::string GetOwner() const override;
    bool IsHost() const override;

    void WriteAllocationID(std::vector<uint8_t>& allocationIdBitstream) const override;

protected:
    static std::string GenerateUUID();

    std::string objectName;
    std::string uniqueId;
    bool shouldSync;
    std::string ownerID;
    bool hostFlag;
};

#endif // REPLICA_H