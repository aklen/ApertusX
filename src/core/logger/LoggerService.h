#ifndef LOGGERSERVICE_H
#define LOGGERSERVICE_H

#include "interfaces/ILoggerService.h"
#include <iostream>
#include <fruit/fruit.h>

class LoggerService : public ILoggerService {
public:
    INJECT(LoggerService()) = default;
    void Log(const std::string& message) override;
};

#endif // LOGGERSERVICE_H
