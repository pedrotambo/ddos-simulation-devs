#include <string>
#include <random>


#include "server.h"

using namespace std;

// This is for a bug in the simulator that executes output function twice
Real lastJobSent = Real(-1);
float serverHashID;
//

Server::Server(const string &name) : 
    Atomic(name),
    job(addInputPort("job")),
    powerSignal(addInputPort("powerSignal")),
    done(addOutputPort("done")),
    ready(addOutputPort("ready")),
    setupTime(DEFAULT_SETUP_TIME),
    mean(DEFAULT_MEAN),
    initialStatus(SERVER_OFF),
    status(initialStatus),
    turnOffAfterCompletion(false),
    poweringOn(false)
{
    if( ParallelMainSimulator::Instance().existsParameter( description(), "initialStatus" ) )
        initialStatus = ParallelMainSimulator::Instance().getParameter( description(), "initialStatus" );
        status = initialStatus;
        // TODO: check initialStatus is off, busy, or free

    if( ParallelMainSimulator::Instance().existsParameter( description(), "mean" ) )
        mean = stof( ParallelMainSimulator::Instance().getParameter( description(), "mean" ) );

    if( ParallelMainSimulator::Instance().existsParameter( description(), "setupTime" ) )
        setupTime = ParallelMainSimulator::Instance().getParameter( description(), "setupTime" );

    if( ParallelMainSimulator::Instance().existsParameter( description(), "distribution" ) ){
        dist = Distribution::create(ParallelMainSimulator::Instance().getParameter( description(),"distribution" ) );
    } else {
        dist = Distribution::create("exponential");	
    }

    MASSERT( dist ) ;
    for ( register int i = 0; i < dist->varCount(); i++ )
    {
        string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
        dist->setVar( i, str2Value( parameter ) ) ;
    }
}


Model &Server::initFunction()
{
    serverHashID = static_cast< float >( fabs(distribution().get() ) );
    passivate();
    return *this;
}


Model &Server::externalFunction(const ExternalMessage &msg)
{
    Real messageValue = Real::from_value(msg.value());
    updateTimeVariables(msg);

    if (msg.port() == job) {
        this->attendJob(msg);
    } else if (msg.port() == powerSignal and messageValue == POWER_OFF_SIGNAL) {
        this->powerOff();
    } else if (msg.port() == powerSignal and messageValue == POWER_ON_SIGNAL) {
        this->powerOn();
    }
    return *this;
}

Model &Server::internalFunction(const InternalMessage &msg)
{
    if (poweringOn) {// the server finished powering on
        status = SERVER_FREE;
        poweringOn = false;
    } else {
        // a job has been processed
        if (turnOffAfterCompletion){
            status = SERVER_OFF;
            turnOffAfterCompletion = false;
        } else {
            status = SERVER_FREE;
            lastJobSent = jobIDToProcess;
            jobIDToProcess = Real(-1);
        }
    }
    passivate();
    return *this ;
}

Model &Server::outputFunction(const CollectMessage &msg)
{
    if (poweringOn){
        sendOutput(msg.time(), ready, SERVER_READY_MESSAGE);
    } else {
        if (status == SERVER_BUSY && jobIDToProcess != lastJobSent) {// This is for a bug in the simulator that executes output function twice
            sendOutput(msg.time(), done, jobIDToProcess);
            lastJobSent = jobIDToProcess; 
            if (turnOffAfterCompletion) {
                sendOutput(msg.time(), ready, SERVER_OFF_MESSAGE);
            }
        } 
        else if (status == SERVER_OFF && jobIDToProcess != lastJobSent) {
            sendOutput(msg.time(), ready, SERVER_OFF_MESSAGE); 
        } else {
            cerr << "[Server::outputFunction] Error: Simulador ejecutando nuevamente outputFunction" << endl;
            // we have to execute this (internalFunction code) for avoid errors
            if (turnOffAfterCompletion){
                status = SERVER_OFF;
                turnOffAfterCompletion = false;
                sendOutput(msg.time(), ready, SERVER_OFF_MESSAGE); 
            } else {
                status = SERVER_FREE;
                sendOutput(msg.time(), ready, SERVER_OFF_MESSAGE);
            }
        }
    }

    return *this ;
}


void Server::attendJob(const ExternalMessage &msg){
    if (status == SERVER_FREE){
        float processing_time = static_cast< float >( fabs(distribution().get() ) );
        VTime job_processing_time = VTime(processing_time);
        jobIDToProcess = Real::from_value(msg.value());
        status = SERVER_BUSY;
        holdIn(AtomicState::active, job_processing_time);
    } else if(status == SERVER_OFF) {
        MTHROW(MException("Llego mensaje a servidor apagado"))
    } else if (status == SERVER_BUSY) {
        MTHROW(MException("[SERVER::attendJob] Llego mensaje a servidor ocupado"))
    } else {
        MTHROW(MException("[SERVER::attendJob] estado invalido"))
    }	
}

void Server::powerOff(){
    if (SERVER_DEBUGGING_ENABLED) {
        cout << "[SERVER::powerOff] APAGANDOME" << endl;
    }

    if (status == SERVER_FREE) {
        status = SERVER_OFF;
        holdIn(AtomicState::active, VTime::Zero);
    } else if (status == SERVER_BUSY) { // Si estaba en busy y me llega que me apague, espero a terminarlo y después me apago
        turnOffAfterCompletion = true;
        holdIn(AtomicState::active, timeLeft);
    } else if(status == SERVER_OFF) {
        MTHROW(MException("[SERVER::powerOff] Apagado a servidor apagado"))
    } else {
        MTHROW(MException("[SERVER::powerOff] estado invalido"))
    }
}

void Server::powerOn(){
    if (SERVER_DEBUGGING_ENABLED) {
        cout << "[SERVER::powerOn] PRENDIENDOME" << endl;
    }
    if(status == SERVER_OFF) {
        if (poweringOn) {
            MTHROW(MException("[SERVER::powerOn] Doble mensaje de prendido"))
        }
        poweringOn = true;
        holdIn(AtomicState::active, setupTime);
    } else if (status == SERVER_BUSY) {
        MTHROW(MException("[SERVER::powerOn] Prendido a servidor busy"))
    } else if (status == SERVER_FREE){
        MTHROW(MException("[SERVER::powerOn] Prendido a servidor free"))
    } else {
        MTHROW(MException("[SERVER::powerOn] estado invalido"))
    }
}

void Server::updateTimeVariables(const ExternalMessage &msg){
    timeLeft = nextChange();
    elapsed = msg.time() - lastChange();
    sigma = elapsed + timeLeft;
}
