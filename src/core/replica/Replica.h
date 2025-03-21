#ifndef REPLICA_H
#define REPLICA_H

#include <vector>
#include <string>

class Replica : public IReplica {
public:
    Replica(std::string name, bool sync, std::string owner, bool isHost) 
        : objectName(std::move(name)), shouldSync(sync), ownerHost(std::move(owner)), isHost(isHost) {}

    virtual ~Replica() = default;

    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual void Deserialize(const std::vector<uint8_t>& data) = 0;

    std::string GetName() const { return objectName; }
    bool IsSynchronized() const { return shouldSync; }
    std::string GetOwnerHost() const { return ownerHost; }
    bool IsHost() const { return isHost; }

private:
    std::string objectName;
    bool shouldSync;
    std::string ownerHost;
    bool isHost;
};

#endif // REPLICA_H
