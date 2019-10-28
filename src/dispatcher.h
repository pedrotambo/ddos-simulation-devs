
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
#include <vector>
#include <map>
#include <set>

#define DISPATCHER_ATOMIC_MODEL_NAME "Dispatcher"


class Dispatcher : public Atomic {
/*
@PringlesModelMetadata
name: Dispatcher
input_ports: newJob, jobDone, serverStatus
output_ports: requestJob, server0, server1, server2, server3, server4, server5, server6, server7, server8, server9
*/

// For Pringles if you want to add ports, use a_server.add_port('serverName')

  public:
    Dispatcher(const string &name = DISPATCHER_ATOMIC_MODEL_NAME );
    virtual string className() const {return DISPATCHER_ATOMIC_MODEL_NAME;}

  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );

    bool hasFreeServer();
    void attendNewJob();
    void attendJobDone(const ExternalMessage &msg);
    void attendNewStackServerInfo(const ExternalMessage &msg);
    int getNextServerToDispatch();
    int numberOfServersFree();
    void printServerStatus();



  private:
    // In Ports
    // This port will receive jobs from the queue
    const Port &newJob;
    // This port will receive a message from server X, informing that a job was done, i.e., [2] => server 2 finished the job.
    const Port &jobDone;
    // This port will receive status info of the stack of servers, i.e., [1, FREE] => now server 1 is free, waiting for jobs.
    const Port &serverStatus;
    // Out Ports
    // This is a map of server_id => port
    map<int, Port*> jobsToProcess;
    // This port will request the queue to send a new job
    Port &requestJob;

    
    // Lifetime programmed since the last state transition to the next planned internal transition.
    VTime sigma;	
    // Time elapsed since the last state transition until now
    VTime elapsed;
    // Time remaining to complete the last programmed Lifetime
    VTime timeLeft;


    // Parmeters
    // Number of servers of the datacenter
    int numberOfServers;
    // State of the servers, serverX: off/free/busy
    map<int, string> statusOfServers;

    // Internal variables
    // The pair key, value: <JobID, ServerID> indicates the server that is processing a specific job
    map<Real, int> jobProcessingServer;
    Real jobID;
    bool requestedJob;
    bool jobArrived;
    int serverToDispatch;
    bool needToRequestJob;

    // Este vector sirve para acordarse que servidor hay que marcar como apagdo cuando termine el job
    set<int> markAsOffWhenDone;

    void updateTimeVariables(const ExternalMessage &);

};

#endif


