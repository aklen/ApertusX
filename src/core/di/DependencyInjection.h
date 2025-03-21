#ifndef DEPENDENCYINJECTION_H
#define DEPENDENCYINJECTION_H

#include <fruit/fruit.h>
#include "interfaces/IEventService.h"
#include "interfaces/ILoggerService.h"
#include "interfaces/IConfigService.h"
#include "interfaces/IPluginService.h"
#include "interfaces/IReplicaService.h"

#include "../event/EventService.h"
#include "../logger/LoggerService.h"
#include "../config/ConfigService.h"
#include "../plugin/PluginService.h"
#include "../replica/ReplicaService.h"

fruit::Component<IEventService, ILoggerService, IConfigService, IPluginService, IReplicaService> getApertusComponent();

#endif // DEPENDENCYINJECTION_H
