#include "AudioReplica.h"
#include <sstream>

AudioReplica::AudioReplica() 
    : Replica("AudioReplica", true, "unknown", false), streamUrl(""), volume(1.0f) {}

AudioReplica::AudioReplica(const std::string& name, bool sync, const std::string& owner, bool isHost,
                           std::string streamUrl, float volume)
    : Replica(name, sync, owner, isHost), streamUrl(std::move(streamUrl)), volume(volume) {}

std::vector<uint8_t> AudioReplica::Serialize() const {
    std::ostringstream oss;
    oss << streamUrl << '\0' << volume;
    std::string data = oss.str();
    return std::vector<uint8_t>(data.begin(), data.end());
}

void AudioReplica::Deserialize(const std::vector<uint8_t>& data) {
    std::istringstream iss(std::string(data.begin(), data.end()));
    std::getline(iss, streamUrl, '\0');
    iss >> volume;
}

void AudioReplica::SetStreamUrl(const std::string& url) {
    streamUrl = url;
}

std::string AudioReplica::GetStreamUrl() const {
    return streamUrl;
}

void AudioReplica::SetVolume(float vol) {
    volume = vol;
}

float AudioReplica::GetVolume() const {
    return volume;
}
