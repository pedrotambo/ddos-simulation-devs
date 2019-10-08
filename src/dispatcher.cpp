#include <string>
#include "dispatcher.h"

using namespace std;

VTime timeToDispatchJobs = VTime::Zero;
VTime timeToRequestJobs = VTime::Zero;

Dispatcher::Dispatcher(const string &name) : 
    Atomic(name),
	newJob(addInputPort("newJob")),
	jobDone(addInputPort("jobDone")),
	serverStackInfo(addInputPort("serverStackInfo")),	
	requestJob(addOutputPort("requestJob")),
	jobID(Real(0)),
	requestedJob(false),
	jobArrived(false)
{
	numberOfServers = stoi( ParallelMainSimulator::Instance().getParameter( description(), "numberOfServers" ) );

	for (register int i = 0; i < numberOfServers; i++){
		string serverStateVarName = "server" + to_string(i);
		// now we initialize server status
		if( ParallelMainSimulator::Instance().existsParameter( description(), serverStateVarName ) ){
			string serverState( ParallelMainSimulator::Instance().getParameter( description(), serverStateVarName ) ) ;
			statusOfServers[i] = serverState;
		} else {
			statusOfServers[i] = DEFAULT_SERVER_STATE;
		}

		// now we create output ports to servers
		string portName = "server" + to_string(i);
		Port* linkToServer = &addOutputPort(portName);
		jobsToProcess[i] = linkToServer;
	}

	if (statusOfServers.size() > numberOfServers){
		MTHROW(MException("Error! Hay más servers que los permitidos"))
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
	// Si hay servers free, nos programamos para pedir
	requestedJob = false;
	jobArrived = false;
	if (this->hasFreeServer()){
		holdIn(AtomicState::active, timeToRequestJobs); 
	} else {
		passivate();	
	}
  	cout << "Dispatcher atomic initialized" << endl;

	return *this;
}


Model &Dispatcher::externalFunction(const ExternalMessage &msg)
{
	// double message_valuea = message_value.value();
	updateTimeVariables(msg);


	if (msg.port() == newJob) {
		this->attendNewJob();
	} else if (msg.port() == jobDone){
		this->attendJobDone(msg);
	} else if(msg.port() == serverStackInfo){
		this->attendNewStackServerInfo(msg);
	}


	return *this;
}

void Dispatcher::attendNewJob(){
	if (not hasFreeServer() or not requestedJob){
		if (not requestedJob){
			MTHROW(MException("Llega un job nuevo pero no fue pedido"));
		} else{
			MTHROW(MException("Llega un job nuevo pero no hay servers libres"));
		}
	} else {
		// Llegó un nuevo job y hay servers libres, me programo para enviarlo
		cout << "New Job arrived, id assigned: " << this->jobID << endl;
		requestedJob = false;
		jobArrived = true;
		holdIn(AtomicState::active, timeToDispatchJobs);
	}
}

void Dispatcher::attendJobDone(const ExternalMessage &msg){
	// Un servidor está informando que termino el job, por lo tanto ahora está libre.
	// Lo libero y me programo para pedir otro, en caso de no haberlo pedido ya
	Real jobIDDone = Real::from_value(msg.value());
	if (jobProcessingServer.count(jobIDDone) == 0){
		MTHROW(MException("LLegó job hecho pero el dispatcher no sabe quien lo estaba procesando"));
	} else {
		int serverThatProcessedTheJob = jobProcessingServer[jobIDDone];
		jobProcessingServer.erase(jobIDDone);
		statusOfServers[serverThatProcessedTheJob] = SERVER_FREE;

		if (requestedJob){
			// si ya pedí un job (y todavía no llego) me pasivo
			passivate();
		} else {
			// si no, me programo para pedir otro
			holdIn(AtomicState::active, timeToRequestJobs);	
		}
		
	}
}

void Dispatcher::attendNewStackServerInfo(const ExternalMessage &msg){
	// resolver que hacer
	MTHROW(MException("resolver este caso"))
}




Model &Dispatcher::internalFunction(const InternalMessage &)
{

	passivate();
	return *this ;
}


Model &Dispatcher::outputFunction(const CollectMessage &msg)
{

	if (this->hasFreeServer() and jobArrived) {
		// si jobArrived es true, es porque vengo de un mensaje de attendNewJob
		int serverToDispatch = getNextServerToDispatch();
		cout << "Server: "<<  serverToDispatch << " - Processing Job: " << this->jobID << endl;
		sendOutput(msg.time(), *jobsToProcess[serverToDispatch], this->jobID);
		statusOfServers[serverToDispatch] = SERVER_BUSY;
		jobProcessingServer[this->jobID] = serverToDispatch;

		jobArrived = false;

		// now we set the id for the next job
		Real nextIDJobToProcess = this->jobID + Real(1);
		this->jobID = nextIDJobToProcess;
	}


	if (this->hasFreeServer() and not requestedJob){
		// Si todavía hay servers libres, pido más jobs
		requestedJob = true;
		sendOutput(msg.time(), requestJob, Real(1));
	}
	
	
	return *this ;
}


void Dispatcher::updateTimeVariables(const ExternalMessage &msg){
	timeLeft = nextChange();
	elapsed = msg.time() - lastChange();
	sigma = elapsed + timeLeft;
}

bool Dispatcher::hasFreeServer(){

	bool freeServers = false;
	for (int i = 0; i < this->numberOfServers; i++){
		if (statusOfServers[i] == SERVER_FREE) {
			freeServers = true;
			break;
		}
 	}
 	return freeServers;
}

void Dispatcher::printServerStatus(){
	for (int i = 0; i < this->numberOfServers; i++){
		cout << "Server " + to_string(i) + ": " + statusOfServers[i] << endl;
	}

}

int Dispatcher::getNextServerToDispatch(){
	int serverToDispatch = -1;
	for (int i = 0; i < this->numberOfServers; i++){
		if (statusOfServers[i] == SERVER_FREE){
			serverToDispatch = i;
			break;
		}
	}
	return serverToDispatch;
}

