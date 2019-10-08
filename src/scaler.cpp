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
    server_farm_size(10),
    exponential_weight(0.6),
    load_moving_avg(0.0),
    load_lower_bound(LOAD_LOWER_BOUND),
    load_upper_bound(LOAD_UPPER_BOUND),
    load_updates_to_break_idle(LOAD_UPDATES_TO_BREAK_IDLE)
{
    if( ParallelMainSimulator::Instance().existsParameter( description(), "server_farm_size" ) ) {
        server_farm_size = stoi( ParallelMainSimulator::Instance().getParameter( description(), "server_farm_size" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "load_lower_bound" ) ) {
        load_lower_bound = stod( ParallelMainSimulator::Instance().getParameter( description(), "load_lower_bound" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "load_upper_bound" ) ) {
        load_upper_bound = stod( ParallelMainSimulator::Instance().getParameter( description(), "load_upper_bound" ) );
    }

    if( ParallelMainSimulator::Instance().existsParameter( description(), "load_updates_to_break_idle" ) ) {
            load_updates_to_break_idle = stoi( ParallelMainSimulator::Instance().getParameter( description(), "load_updates_to_break_idle" ) );
    }

    idle_updates_left = 0;

    for (auto idx = 0; idx < server_farm_size; idx++) {
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
        idle_updates_left = idle_updates_left == 0 ? 0 : idle_updates_left - 1;
        passivate();
    } else if (msg.port() == serverResponse) {
        has_server_update = true;
        idle_updates_left = load_updates_to_break_idle;
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

    if (shouldPowerOffServer()) {
        signaling_server = true;
        auto available_server = getPoweredOffServer();
        server_update = {Real(available_server), POWER_OFF_SIGNAL};
    }

    if (shouldPowerOnServer()) {
        signaling_server = true;
        auto available_server = getPoweredOnServer();
        server_update = {Real(available_server), POWER_ON_SIGNAL};
    }

    passivate();
    return *this ;
}


Model &AutoScaler::outputFunction(const CollectMessage &msg)
{
    if (has_server_update){
        sendOutput(msg.time(), serverStatus, server_update);
    }

    if (shouldPowerOffServer()) {
        Tuple<Real> outValue{POWER_OFF_SIGNAL};
        auto available_server = getPoweredOffServer();
        sendOutput(msg.time(), *servers[available_server], outValue);
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
    load_moving_avg = exponential_weight * new_val + (1 - exponential_weight) * load_moving_avg;
}

int AutoScaler::runningServers() {
    int count;
    for (const auto& pair : server_status) {
        if (pair.second == "on") {
            count += 1;
        }
    }
}

bool AutoScaler::shouldPowerOffServer() {
    return load_moving_avg > LOAD_UPPER_BOUND && idle_updates_left > 0 && runningServers() > 1 && getPoweredOffServer() != -1;
}

bool AutoScaler::shouldPowerOnServer() {
    return load_moving_avg < LOAD_LOWER_BOUND && idle_updates_left > 0 && getPoweredOnServer() != -1;
}