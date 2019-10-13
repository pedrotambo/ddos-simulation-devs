#include "pmodeladm.h"
#include "register.h"
#include "constants.h"

#include "server.h"
#include "dispatcher.h"
#include "queue.h"
#include "scaler.h"
#include "attacker.h"

void register_atomics_on(ParallelModelAdmin &admin)
{
    admin.registerAtomic(NewAtomicFunction<Server>(), SERVER_ATOMIC_MODEL_NAME);
    admin.registerAtomic(NewAtomicFunction<Dispatcher>(), DISPATCHER_ATOMIC_MODEL_NAME);
    admin.registerAtomic(NewAtomicFunction<ServerQueue>(), SERVER_QUEUE_ATOMIC_MODEL_NAME);
    admin.registerAtomic(NewAtomicFunction<AutoScaler>(), AUTO_SCALER_ATOMIC_MODEL_NAME);
    admin.registerAtomic(NewAtomicFunction<Attacker>(), ATTACKER_ATOMIC_MODEL_NAME);
    
}
