
#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <random>
#include <math.h>
#include "atomic.h"
#include "VTime.h"
#include "message.h"
#include "parsimu.h"
#include "constants.h"
#include "except.h"

#define DISPATCHER_ATOMIC_MODEL_NAME "Dispatcher"


class Dispatcher : public Atomic {
/*
@ModelMetadata
name: Dispatcher
*/

  public:
    Dispatcher(const string &name = DISPATCHER_ATOMIC_MODEL_NAME );
    virtual string className() const {return DISPATCHER_ATOMIC_MODEL_NAME;}

  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );


  private:
    // In Ports
    // This port will receive jobs from the queue
    const Port &job;
    // This port will receive status info of the stack of servers
    const Port &serverStackInfo;
    // Out Ports
    // This port will output the next server to process a job, the connections between load balancer and servers
    // have to be done in the .ma
    Port &jobToProcess;

    
    // Lifetime programmed since the last state transition to the next planned internal transition.
    VTime sigma;	
    // Time elapsed since the last state transition until now
    VTime elapsed;
    // Time remaining to complete the last programmed Lifetime
    VTime timeLeft;


    // // Internal variables
    // Number of servers of the datacenter
    int numberOfServers;
    // State of the servers
    map<int, string> statusOfServers;


    void updateTimeVariables(const ExternalMessage &);

};

#endif


