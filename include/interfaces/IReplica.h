#ifndef IREPLICA_H
#define IREPLICA_H

#include <vector>
#include <string>

class IReplica {
public:
    virtual ~IReplica() = default;

    virtual std::vector<uint8_t> Serialize() const = 0;
    virtual void Deserialize(const std::vector<uint8_t>& data) = 0;

    virtual std::string GetName() const = 0;
    virtual bool IsSynchronized() const = 0;
    virtual std::string GetOwnerHost() const = 0;
    virtual bool IsHost() const = 0;
};

#endif // IREPLICA_H
