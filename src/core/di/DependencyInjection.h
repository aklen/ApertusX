#ifndef DEPENDENCYINJECTION_H
#define DEPENDENCYINJECTION_H

#include <fruit/fruit.h>
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include "interfaces/IConfigService.h"
#include "interfaces/IPluginService.h"
#include "../event/EventService.h"
#include "../logger/LoggerService.h"
#include "../config/ConfigService.h"
#include "../plugin/PluginService.h"

fruit::Component<IEventService, ILoggerService, IConfigService, IPluginService> getApertusComponent();

#endif // DEPENDENCYINJECTION_H
