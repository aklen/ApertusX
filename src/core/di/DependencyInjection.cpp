#include "DependencyInjection.h"

fruit::Component<
    IEventService,
    ILoggerService,
    IConfigService,
    IPluginService
    // IReplicaService,
    // IReplicaFactory,
    // IReplicaRegistrar,
    > getApertusComponent() {
    return fruit::createComponent()
        .bind<IEventService, EventService>()
        .bind<ILoggerService, LoggerService>()
        .bind<IConfigService, ConfigService>()
        .bind<IPluginService, PluginService>();
        // .bind<IReplicaService, ReplicaService>()
        // .bind<IReplicaFactory, ReplicaFactory>()
        // .bind<IReplicaRegistrar, ReplicaRegistrar>();
}
