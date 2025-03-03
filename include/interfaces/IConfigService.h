#ifndef ICONFIGSERVICE_H
#define ICONFIGSERVICE_H

#include <string>

class IConfigService {
public:
    virtual ~IConfigService() = default;
    virtual void LoadConfig(const std::string& filename) = 0;
};

#endif // ICONFIGSERVICE_H
