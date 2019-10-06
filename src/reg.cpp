#include "pmodeladm.h"
#include "register.h"
#include "constants.h"
#include "server.h"
#include "dispatcher.h"



void register_atomics_on(ParallelModelAdmin &admin)
{
        admin.registerAtomic(NewAtomicFunction<Server>(), SERVER_ATOMIC_MODEL_NAME);
        admin.registerAtomic(NewAtomicFunction<Dispatcher>(), DISPATCHER_ATOMIC_MODEL_NAME);
}
