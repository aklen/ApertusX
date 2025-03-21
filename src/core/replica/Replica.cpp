#include "Replica.h"
#include <sstream>
#include <iomanip>
#include <random>

Replica::Replica(std::string name, bool sync, std::string owner, bool isHost)
    : objectName(std::move(name)), shouldSync(sync), ownerID(std::move(owner)), hostFlag(isHost) {
    uniqueId = GenerateUUID();
}

std::string Replica::GetName() const {
    return objectName;
}

std::string Replica::GetUniqueId() const {
    return uniqueId;
}

bool Replica::IsSynchronized() const {
    return shouldSync;
}

std::string Replica::GetOwner() const {
    return ownerID;
}

bool Replica::IsHost() const {
    return hostFlag;
}

void Replica::WriteAllocationID(std::vector<uint8_t>& allocationIdBitstream) const {
    std::ostringstream oss;
    oss << objectName << '\0' << ownerID << '\0' << (hostFlag ? "1" : "0");
    std::string data = oss.str();
    allocationIdBitstream.assign(data.begin(), data.end());
}

std::string Replica::GenerateUUID() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << part1
        << "-"
        << std::hex << std::setw(16) << std::setfill('0') << part2;
    
    return oss.str();
}
