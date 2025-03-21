#pragma once
#include "IReplicaService.h"
#include "IReplica.h"
#include "Replica.h"
#include "AudioReplica.h"
#include <unordered_map>
#include <arpa/inet.h>

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh> // netAddressBits
// #include <FramedMemoryBufferSource.hh> // FramedMemoryBufferSource

class ReplicaService : public IReplicaService {
public:
    ReplicaService(const std::string& rtpAddr, uint16_t port)
        : rtpAddress(rtpAddr), rtpPort(port) {
        env = BasicUsageEnvironment::createNew(*scheduler);
    }

    void RegisterReplica(std::shared_ptr<IReplica> replica) {
        replicas[replica->GetName()] = replica;
    } 

    void SendReplica(const IReplica& replica) override {
        const auto& concreteReplica = dynamic_cast<const IReplica&>(replica);
        if (!concreteReplica.IsSynchronized()) return;
    
        auto data = concreteReplica.Serialize();
        OutPacketBuffer::maxSize = data.size();
    
        // Convert the RTP address to struct sockaddr_storage
        struct sockaddr_storage groupAddr = {};
        auto* addr_in = reinterpret_cast<struct sockaddr_in*>(&groupAddr);
        addr_in->sin_family = AF_INET;
        addr_in->sin_addr.s_addr = inet_addr(rtpAddress.c_str());
        addr_in->sin_port = htons(rtpPort);
    
        // Create the Groupsock object
        Groupsock rtpGroupsock(*env, groupAddr, Port(rtpPort), 255); // TTL = 255
        rtpSink = SimpleRTPSink::createNew(*env, &rtpGroupsock, 96, 90000, "application", "octet-stream", 1, True, False);
    
        // Create a MediaSource (e.g., ByteStreamFileSource)
        FramedSource* mediaSource = ByteStreamFileSource::createNew(*env, "example.mp4", False);
        if (mediaSource == nullptr) {
            (*env) << "Failed to create media source\n";
            return;
        }
    
        // Start playing the media source
        rtpSink->startPlaying(*mediaSource, nullptr, nullptr);
    }

    void ReceiveReplica() {
        // Convert the RTP address to struct sockaddr_storage
        // struct sockaddr_storage groupAddr = {};
        // auto* addr_in = reinterpret_cast<struct sockaddr_in*>(&groupAddr);
        // addr_in->sin_family = AF_INET;
        // addr_in->sin_addr.s_addr = inet_addr(rtpAddress.c_str());
        // addr_in->sin_port = htons(rtpPort);
    
        // // Create the Groupsock object
        // Groupsock rtpGroupsock(*env, groupAddr, Port(rtpPort), 255); // TTL = 255
    
        // // Create the RTP source
        // rtpSource = MultiFramedRTPSource::createNew(*env, &rtpGroupsock, 96);
        // if (rtpSource == nullptr) {
        //     (*env) << "Failed to create RTP source\n";
        //     return;
        // }
    
        // // Start receiving frames
        // rtpSource->getNextFrame(buffer, sizeof(buffer), ReplicaCallback, this, nullptr, nullptr);
    
        // // Enter the event loop to process incoming data
        // env->taskScheduler().doEventLoop();
    }

    static void ReplicaCallback(void* clientData, unsigned frameSize, unsigned, struct timeval, unsigned) {
        // auto* manager = static_cast<IReplicaService*>(clientData);
        // std::vector<uint8_t> data(manager->buffer, manager->buffer + frameSize);

        // std::string objectName;
        // AudioReplica::Extract(data, objectName);

        // if (manager->replicas.find(objectName) != manager->replicas.end()) {
        //     manager->replicas[objectName]->Deserialize(data);
        // } else {
        //     std::shared_ptr<AudioReplica> newReplica = std::make_shared<AudioReplica>(objectName, true, "network", false, "", 0.0f);
        //     newReplica->Deserialize(data);
        //     manager->replicas[objectName] = newReplica;
        // }

        // manager->replicas[objectName]->Print();
    }

    unsigned char buffer[4096];

private:
    std::string rtpAddress;
    uint16_t rtpPort;
    UsageEnvironment* env;
    RTPSink* rtpSink;
    RTPSource* rtpSource;
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();

    std::unordered_map<std::string, std::shared_ptr<IReplica>> replicas;
};
