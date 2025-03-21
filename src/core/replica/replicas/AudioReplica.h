#ifndef AUDIO_REPLICA_H
#define AUDIO_REPLICA_H

#include "replica/Replica.h"
#include <fruit/fruit.h>

class AudioReplica : public Replica {
public:
    AudioReplica();
    AudioReplica(const std::string& name, bool sync, const std::string& owner, bool isHost, 
                 std::string streamUrl, float volume);

    std::vector<uint8_t> Serialize() const override;
    void Deserialize(const std::vector<uint8_t>& data) override;

    void SetStreamUrl(const std::string& url);
    std::string GetStreamUrl() const;

    void SetVolume(float volume);
    float GetVolume() const;

private:
    std::string streamUrl;
    float volume;
};

#endif // AUDIO_REPLICA_H
