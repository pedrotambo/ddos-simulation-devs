
#ifndef _ServerQueue_H_
#define _ServerQueue_H_

#include <vector>
#include <queue>

#include "atomic.h"
#include "constants.h"
#include "VTime.h"

#define SERVER_QUEUE_ATOMIC_MODEL_NAME "ServerQueue"

class ServerQueue : public Atomic {
/*
@PringlesModelMetadata
name: ServerQueue
input_ports: in, emit
output_ports: out, discarded, current_size
*/

    public:
        typedef std::list<value_ptr> ElementList;

        ServerQueue(const string &name = SERVER_QUEUE_ATOMIC_MODEL_NAME );
        virtual string className() const {return SERVER_QUEUE_ATOMIC_MODEL_NAME;}

    protected:
        Model &initFunction();
        Model &externalFunction( const ExternalMessage & );
        Model &internalFunction( const InternalMessage & );
        Model &outputFunction( const CollectMessage & );

    private:
        // Lifetime programmed since the last state transition to the next planned internal transition.
        VTime sigma;
        // Time elapsed since the last state transition until now
        VTime elapsed;	

        // internal variables
        VTime previous_sigma; // Para reestablecer después de forzar transiciones internas
        VTime currentSizeFrequency;
        uint size;
        bool emition_pending;
        ElementList dropped_jobs; //Ids of the job being dropped
        ElementList queue;

        const Port& in;
        const Port& emit;
        Port& out;
        Port& discarded;
        Port& current_size;
};

#endif