#ifndef CONFIGSERVICE_H
#define CONFIGSERVICE_H

#include "interfaces/IConfigService.h"
#include <iostream>
#include <fruit/fruit.h>

class ConfigService : public IConfigService {
public:
    INJECT(ConfigService()) = default;
    void LoadConfig(const std::string& filename) override;
};

#endif // CONFIGSERVICE_H
