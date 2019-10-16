#include <string>
#include <random>


#include "scaler.h"
#include "constants.h"

#define DEBUG_STR "[SCALER]"
using namespace std;


AutoScaler::AutoScaler(const string &name) : 
    Atomic(name),
    queueLoad(addInputPort("queueLoad")),
    serverResponse(addInputPort("serverResponse")),
    serverStatus(addOutputPort("serverStatus")),
    numberOfServers(10),
    exponentialWeight(0.6),
    idle_updates_left(0),
    load_moving_avg(0.0),
    loadLowerBound(LOAD_LOWER_BOUND),
    loadUpperBound(LOAD_UPPER_BOUND),
    loadUpdatesToBreakIdle(LOAD_UPDATES_TO_BREAK_IDLE)
{
    if( ParallelMainSimulator::Instance().existsParameter( description(), "numberOfServers" ) ) {
        numberOfServers = stoi( ParallelMainSimulator::Instance().getParameter( description(), "numberOfServers" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "loadLowerBound" ) ) {
        loadLowerBound = stod( ParallelMainSimulator::Instance().getParameter( description(), "loadLowerBound" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "loadUpperBound" ) ) {
        loadUpperBound = stod( ParallelMainSimulator::Instance().getParameter( description(), "loadUpperBound" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "exponentialWeight" ) ) {
        exponentialWeight = stod( ParallelMainSimulator::Instance().getParameter( description(), "exponentialWeight" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "loadUpdatesToBreakIdle" ) ) {
        loadUpdatesToBreakIdle = stoi( ParallelMainSimulator::Instance().getParameter( description(), "loadUpdatesToBreakIdle" ) );
    }

    has_server_update = false;

    for (auto idx = 0; idx < numberOfServers; idx++) {
        string server_name = "server" + to_string(idx);
        Port* linkToServer = &addOutputPort(server_name);
        servers[idx] = linkToServer;

        if( ParallelMainSimulator::Instance().existsParameter( description(), server_name ) ) {
            string server = ParallelMainSimulator::Instance().getParameter( description(), server_name );
            server_status[idx] = server;
        } else {
            server_status[idx] = "off";
        }
    }
}


Model &AutoScaler::initFunction()
{
    passivate();
    return *this;
}


Model &AutoScaler::externalFunction(const ExternalMessage &msg)
{
    if (msg.port() == queueLoad) {
        double valueDouble = stod(msg.value()->asString());

        this->updateLoadFactor(valueDouble);

        if (idle_updates_left == 0) {
            if (shouldPowerOffServer()) {
                auto available_server = getPoweredOnServer();
                server_update = {Real(available_server), POWER_OFF_SIGNAL};
                signaling_server = true;
                holdIn(AtomicState::active, VTime::Zero);
            } 
            
            if (shouldPowerOnServer()) {
                auto available_server = getPoweredOffServer();
                server_update = {Real(available_server), POWER_ON_SIGNAL};
                signaling_server = true;
                holdIn(AtomicState::active, VTime::Zero);
            }
        } else {
            passivate();
        }

        if (!signaling_server) {
            //Updates of load factor of queue left 
            //shouldn't start updating until new server is functioning
            idle_updates_left = idle_updates_left == 0 ? 0 : idle_updates_left - 1;
        }
    } else if (msg.port() == serverResponse) {
        has_server_update = true;
        holdIn(AtomicState::active, VTime::Zero);
    }
    return *this;
}

Model &AutoScaler::internalFunction(const InternalMessage &)
{

    if (has_server_update) {
        updateServerStatus(server_update);
        has_server_update = false;
        signaling_server = false;
    }

    if (shouldPowerOnServer()) {
        updateServerStatus(server_update);
        idle_updates_left = loadUpdatesToBreakIdle;
    }

    if (shouldPowerOffServer()) {
        updateServerStatus(server_update);
        idle_updates_left = loadUpdatesToBreakIdle;
    }

    passivate();
    return *this ;
}


Model &AutoScaler::outputFunction(const CollectMessage &msg)
{
    if (has_server_update && server_update[1] == POWER_OFF_SIGNAL){
        signaling_server = false;
        cerr << "[SCALER]" << msg.time() << "  " << "  " << server_update << "  " << endl;
        sendOutput(msg.time(), serverStatus, server_update);
    }

    if (shouldPowerOffServer()) {
        cerr << "[SCALER]" << msg.time() << "  " << (int)server_update[0].value() << "  " << POWER_OFF_SIGNAL << "  " << endl;
        cerr << "[SCALER]" << msg.time() << "  " << "  " << server_update << "  " << endl;
        sendOutput(msg.time(), *servers[(int)server_update[0].value()], POWER_OFF_SIGNAL);
        sendOutput(msg.time(), serverStatus, server_update);
    }

    if (shouldPowerOnServer()) {
        cerr << "[SCALER]" << msg.time() << "  " << (int)server_update[0].value() << "  " << POWER_ON_SIGNAL << "  " << endl;       
        sendOutput(msg.time(), *servers[(int)server_update[0].value()], POWER_ON_SIGNAL);
    }

    return *this;
}

void AutoScaler::updateServerStatus(const Tuple<Real>& server_update){
    string new_status = (server_update[1] == POWER_OFF_SIGNAL) ? "off" : "free";
    server_status[(uint)server_update[0].value()] = new_status;
}

int AutoScaler::getPoweredOffServer() {
    for(uint idx = 0; idx < server_status.size(); idx++){
        if (server_status[idx] == "off") {
            return idx;
        }
    } 

    return -1;
}

int AutoScaler::getPoweredOnServer() {
    for(uint idx = 0; idx < server_status.size(); idx++) 
        if (server_status[idx] == "free") 
            return idx;

    return -1;
}

void AutoScaler::updateLoadFactor(double new_val) {
    load_moving_avg = exponentialWeight * new_val + (1 - exponentialWeight) * load_moving_avg;
}

int AutoScaler::runningServers() {
    int count = 0;
    for (const auto& pair : server_status) {
        if (pair.second == "free") {
            count += 1;
        }
    }
    return count;
}

bool AutoScaler::shouldPowerOffServer() {
    return load_moving_avg < loadLowerBound && idle_updates_left == 0 && runningServers() > 1 && getPoweredOnServer() != -1;
}

bool AutoScaler::shouldPowerOnServer() {
    return load_moving_avg > loadUpperBound && idle_updates_left == 0 && getPoweredOffServer() != -1;
}
