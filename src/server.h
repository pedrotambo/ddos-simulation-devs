
#ifndef _SERVER_H_
#define _SERVER_H_

#include <random>
#include <math.h>
#include "atomic.h"
#include "VTime.h"
#include "distri.h"
#include "message.h"
#include "parsimu.h"
#include "constants.h"
#include "except.h"

#define SERVER_ATOMIC_MODEL_NAME "Server"


class Server : public Atomic {
/*
@ModelMetadata
name: Server
*/

  public:
    Server(const string &name = SERVER_ATOMIC_MODEL_NAME );
    virtual string className() const {return SERVER_ATOMIC_MODEL_NAME;}

  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );

    void attendJob();
    void powerOff();
    void powerOn();

  private:
    // In Ports
    const Port &job;
    const Port &powerSignal;
    // Out Ports
    Port &done;
    Port &ready;

    
    // Lifetime programmed since the last state transition to the next planned internal transition.
    VTime sigma;	
    // Time elapsed since the last state transition until now
    VTime elapsed;
    // Time remaining to complete the last programmed Lifetime
    VTime timeLeft;


    // Internal variables
    
    // Time it takes for the server to turn on
    VTime setupTime;
    // Mean of the exponential distribution which says the processing time of a job
    double mean;
    // Status of the server on init: on/off
    string initialStatus;
    // Current status of the server
    string status;
    // Distribution of the proccess time of a job
    Distribution *dist;
    Distribution &distribution(){return *dist;}

    // Flags
    // flag that says that after completion of the job, the server must shut down. This is for
    // external message of shut down while the server is processing a job
    bool turnOffAfterCompletion;
    // Says if the server is powering on or not (during setupTime this value must be true)
    bool poweringOn;
    

    void updateTimeVariables(const ExternalMessage &);
    double get_param(const string &);

};

#endif


