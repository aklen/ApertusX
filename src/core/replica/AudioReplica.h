#pragma once
#include "IReplica.h"
#include "Replica.h"
#include <vector>
#include <string>
#include <iostream>

class AudioReplica : public Replica {
public:
    AudioReplica(std::string name, bool sync, std::string owner, bool isHost, std::string streamId, float playbackTime)
        : Replica(std::move(name), sync, std::move(owner), isHost), streamId(std::move(streamId)), playbackTime(playbackTime) {}

    static void Extract(const std::vector<uint8_t>& data, std::string& value) {
        value = std::string(reinterpret_cast<const char*>(data.data()));
    }

    static void Extract(const std::vector<uint8_t>& data, float& value) {
        value = *reinterpret_cast<const float*>(data.data() + data.size() - sizeof(value));
    }

    std::vector<uint8_t> Serialize() const override {
        std::vector<uint8_t> data;
        Append(data, GetName());
        Append(data, streamId);
        Append(data, playbackTime);
        return data;
    }

    void Deserialize(const std::vector<uint8_t>& data) override {
        Extract(data, streamId);
        Extract(data, playbackTime);
    }

    void Print() const {
        std::cout << "AudioReplica -> Name: " << GetName() 
                  << ", StreamID: " << streamId 
                  << ", PlaybackTime: " << playbackTime 
                  << ", Owner: " << GetOwnerHost() 
                  << ", Sync: " << IsSynchronized() << std::endl;
    }

private:
    std::string streamId;
    float playbackTime;

    static void Append(std::vector<uint8_t>& data, const std::string& value) {
        data.insert(data.end(), value.begin(), value.end());
        data.push_back('\0');
    }

    static void Append(std::vector<uint8_t>& data, float value) {
        auto ptr = reinterpret_cast<const uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(value));
    }
};
