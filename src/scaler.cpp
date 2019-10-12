#include <string>
#include <random>


#include "scaler.h"
#include "constants.h"

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

    for (auto idx = 0; idx < numberOfServers; idx++) {
        string server_name = "server" + to_string(idx);
        servers[idx] = &addOutputPort(server_name);

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
    cout << "AutoScaler atomic initialized" << endl;

    return *this;
}


Model &AutoScaler::externalFunction(const ExternalMessage &msg)
{
    if (msg.port() == queueLoad) {
        double valueDouble = stod(msg.value()->asString());
        this->updateLoadFactor(valueDouble);
        
        cout << "Nuevo AVG: " << load_moving_avg << endl;
        
        if (!signaling_server) {
            //Updates of load factor of queue left 
            //shouldn't start updating until new server is functioning
            idle_updates_left = idle_updates_left == 0 ? 0 : idle_updates_left - 1;
        }

        if (idle_updates_left == 0 && (shouldPowerOffServer() || shouldPowerOnServer())) {
            holdIn(AtomicState::active, VTime::Zero);
        } else {
            passivate();
        }
    } else if (msg.port() == serverResponse) {
        cout << "LLego update del servidor" << endl;
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
        idle_updates_left = loadUpdatesToBreakIdle;
    }
    if (shouldPowerOffServer()) {
        auto available_server = getPoweredOnServer();
        server_update = {Real(available_server), POWER_OFF_SIGNAL};
        updateServerStatus(server_update);
    }

    if (shouldPowerOnServer()) {
        signaling_server = true;
        auto available_server = getPoweredOffServer();
        server_update = {Real(available_server), POWER_ON_SIGNAL};
        cout << "Mando a prender el server: " << available_server << endl;
    }

    passivate();
    return *this ;
}


Model &AutoScaler::outputFunction(const CollectMessage &msg)
{
    if (has_server_update){
        //Only server updates are power on signals
        sendOutput(msg.time(), serverStatus, server_update);
        cout << "Aviso que se prendió el servidor: " << server_update << endl; 
    }

    if (shouldPowerOffServer()) {
        auto available_server = getPoweredOnServer();
        Tuple<Real> powerOffSignal{POWER_OFF_SIGNAL};
        Tuple<Real> newServerUpdate{Real(available_server), POWER_OFF_SIGNAL};

        sendOutput(msg.time(), *servers[available_server], powerOffSignal);
        cout << "Mando a apagar el server: " << available_server << endl;                
        sendOutput(msg.time(), serverStatus, newServerUpdate);
        cout << "Aviso que se apagó el servidor: " << newServerUpdate << endl;
    }

    if (shouldPowerOnServer()) {
        Tuple<Real> outValue{POWER_ON_SIGNAL};
        auto available_server = getPoweredOnServer();
        sendOutput(msg.time(), *servers[available_server], outValue);
    }

    return *this;
}

void AutoScaler::updateServerStatus(const Tuple<Real>& server_update){
    string new_status = (server_update[1] == POWER_OFF_SIGNAL) ? "off" : "on";
    server_status[(uint)server_update[0].value()] = new_status;
}

int AutoScaler::getPoweredOffServer() {
    for(uint idx = 0; idx < server_status.size(); idx++) 
        if (server_status[idx] == "off") 
            return idx;

    return -1;
}

int AutoScaler::getPoweredOnServer() {
    for(uint idx = 0; idx < server_status.size(); idx++) 
        if (server_status[idx] == "on") 
            return idx;

    return -1;
}

void AutoScaler::updateLoadFactor(double new_val) {
    load_moving_avg = exponentialWeight * new_val + (1 - exponentialWeight) * load_moving_avg;
}

int AutoScaler::runningServers() {
    int count = 0;
    for (const auto& pair : server_status) {
        if (pair.second == "on") {
            count += 1;
        }
    }
    return count;
}

bool AutoScaler::shouldPowerOffServer() {
    return !signaling_server && load_moving_avg < loadLowerBound && idle_updates_left == 0 && runningServers() > 1 && getPoweredOnServer() != -1;
}

bool AutoScaler::shouldPowerOnServer() {
    return !signaling_server && (load_moving_avg > loadUpperBound) && (idle_updates_left == 0) && (getPoweredOffServer() != -1);
}