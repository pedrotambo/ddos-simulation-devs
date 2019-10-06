#include "pmodeladm.h"
#include "register.h"
#include "constants.h"
#include "server.h"
#include "queue.h"


void register_atomics_on(ParallelModelAdmin &admin)
{
        admin.registerAtomic(NewAtomicFunction<Server>(), SERVER_ATOMIC_MODEL_NAME);
        admin.registerAtomic(NewAtomicFunction<ServerQueue>(), SERVER_QUEUE_ATOMIC_MODEL_NAME);
}
