#include "DependencyInjection.h"

fruit::Component<IEventService, ILoggerService, IConfigService, IPluginService> getApertusComponent() {
    return fruit::createComponent()
        .bind<IEventService, EventService>()
        .bind<ILoggerService, LoggerService>()
        .bind<IConfigService, ConfigService>()
        .bind<IPluginService, PluginService>();
}
