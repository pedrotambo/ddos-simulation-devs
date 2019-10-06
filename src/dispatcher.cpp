#include <string>
#include "dispatcher.h"

using namespace std;


Dispatcher::Dispatcher(const string &name) : 
    Atomic(name),
	job(addInputPort("job")),
	serverStackInfo(addInputPort("serverStackInfo")),
	jobToProcess(addInputPort("jobToProcess"))
{
	numberOfServers = stoi( ParallelMainSimulator::Instance().getParameter( description(), "numberOfServers" ) );

	for (register int i = 0; i < numberOfServers; i++){
		string serverStateVarName = "server" + to_string(i);

		if( ParallelMainSimulator::Instance().existsParameter( description(), serverStateVarName ) ){
			string serverState( ParallelMainSimulator::Instance().getParameter( description(), serverStateVarName ) ) ;
			statusOfServers[i] = serverState;
		} else {
			statusOfServers[i] = DEFAULT_SERVER_STATE;
		}
	}

	cout << "Dispatcher atomic successfully created" << endl;
	cout << "Dispatcher parameters:" << endl;
	cout << "	numberOfServers: " << numberOfServers << endl;
	for (int i = 0; i < numberOfServers; i++){
		cout << "	statusOfServer " + to_string(i) + ": " << statusOfServers[i] << endl;	
	} 
	
}


Model &Dispatcher::initFunction()
{
	passivate();
  	cout << "Dispatcher atomic initialized" << endl;

	return *this;
}


Model &Dispatcher::externalFunction(const ExternalMessage &msg)
{
	Real message_value = Real::from_value(msg.value());
	updateTimeVariables(msg);

	// if (msg.port() == job) {
	// 	this->attendJob();
	// } else if (msg.port() == powerSignal and message_value == POWER_OFF_SIGNAL) {
	// 	this->powerOff();
	// } else if (msg.port() == powerSignal and message_value == POWER_ON_SIGNAL) {
	// 	this->powerOn();
	// }
	return *this;
}


void Dispatcher::updateTimeVariables(const ExternalMessage &msg){
	timeLeft = nextChange();
	elapsed = msg.time() - lastChange();
	sigma = elapsed + timeLeft;
}

Model &Dispatcher::internalFunction(const InternalMessage &)
{
	// if (poweringOn) {
	// 	// the server finished powering on
	// 	status = "free";
	// 	poweringOn = false;
	// } else {
	// 	// a job has been processed
	// 	if (turnOffAfterCompletion){
	// 		status = "off";
	// 		turnOffAfterCompletion = false;
	// 	} else {
	// 		status = "free";
	// 	}
	// }
	passivate();
	return *this ;
}


Model &Dispatcher::outputFunction(const CollectMessage &msg)
{
	// if (poweringOn){
	// 	Tuple<Real> outValue{SERVER_READY_MESSAGE};
	// 	sendOutput(msg.time(), ready, outValue);
	// } else {
	// 	Tuple<Real> outValue{JOB_DONE_MESSAGE};
	// 	sendOutput(msg.time(), done, outValue);
	// }

	return *this ;
}
