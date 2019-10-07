#include <string>
#include <random>


#include "server.h"

using namespace std;


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
	cout << "Server atomic succesfully created" << endl;

	cout << "Server parameters: " << endl;
	cout << "	mean: " << mean << endl;
	cout << "	setupTime: " << setupTime << endl;
	cout << "	initialStatus: " << initialStatus << endl;
	cout << "	poweringOn: " << poweringOn << endl;

}


Model &Server::initFunction()
{
	passivate();
  	cout << "Server atomic initialized" << endl;

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


void Server::attendJob(const ExternalMessage &msg){
	if(status == SERVER_OFF) {
		cout << "Llego mensaje de job a servidor apagado" << endl;
		MTHROW(MException("Llego mensaje a servidor apagado"))
	} else if (status == SERVER_BUSY) {
		cout << "Llego mensaje a servidor ocupado" << endl;
		MTHROW(MException("Llego mensaje a servidor ocupado"))
	} else if (status == SERVER_FREE){
		float processing_time = static_cast< float >( fabs(distribution().get() ) );
		VTime job_processing_time = VTime(processing_time);
		jobIDToProcess = Real::from_value(msg.value());
		status = SERVER_BUSY;
		holdIn(AtomicState::active, job_processing_time) ;
	} else {
		MTHROW(MException("estado invalido"))
	}	
}

void Server::powerOff(){
	if(status == SERVER_OFF) {
		MTHROW(MException("Apagado a servidor apagado"))
	} else if (status == SERVER_BUSY) {
		turnOffAfterCompletion = true;
	} else if (status == SERVER_FREE){
		status = SERVER_OFF;
		passivate();
	} else {
		MTHROW(MException("estado invalido"))
	}
}

void Server::powerOn(){
	if(status == SERVER_OFF) {
		if (poweringOn) {
			MTHROW(MException("Doble mensaje de prendido"))
		}
		poweringOn = true;
		holdIn(AtomicState::active, setupTime);
	} else if (status == SERVER_BUSY) {
		MTHROW(MException("Prendido a servidor busy"))
	} else if (status == SERVER_FREE){
		MTHROW(MException("Prendido a servidor free"))
	} else {
		MTHROW(MException("estado invalido"))
	}
}

void Server::updateTimeVariables(const ExternalMessage &msg){
	timeLeft = nextChange();
	elapsed = msg.time() - lastChange();
	sigma = elapsed + timeLeft;
}

Model &Server::internalFunction(const InternalMessage &)
{
	if (poweringOn) {
		// the server finished powering on
		status = SERVER_FREE;
		poweringOn = false;
	} else {
		// a job has been processed
		if (turnOffAfterCompletion){
			status = SERVER_OFF;
			turnOffAfterCompletion = false;
		} else {
			status = SERVER_FREE;
		}
	}
	passivate();
	return *this ;
}


Model &Server::outputFunction(const CollectMessage &msg)
{
	if (poweringOn){
		Tuple<Real> outValue{SERVER_READY_MESSAGE};
		sendOutput(msg.time(), ready, outValue);
	} else {
		Tuple<Real> outValue{jobIDToProcess};
		sendOutput(msg.time(), done, outValue);
	}

	return *this ;
}

double Server::get_param(const string &name)
{
    double value = 0;

    try
    {
        string param = ParallelMainSimulator::Instance().getParameter(description(), name);
        stringstream param_stream(param);

        param_stream >> value;
    }
    catch(IniRequestException &)
    {}

    return value;
}