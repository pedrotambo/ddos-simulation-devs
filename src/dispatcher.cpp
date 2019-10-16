#include <string>
#include "dispatcher.h"

using namespace std;

VTime timeToDispatchJobs = VTime::Zero;
VTime timeToRequestJobs = VTime::Zero;


Dispatcher::Dispatcher(const string &name) : 
    Atomic(name),
    newJob(addInputPort("newJob")),
    jobDone(addInputPort("jobDone")),
    serverStatus(addInputPort("serverStatus")),	
    requestJob(addOutputPort("requestJob")),
    jobID(Real(0)),
    requestedJob(false),
    jobArrived(false),
    serverToDispatch(-1)
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
        if (DISPATCHER_DEBUGGING_ENABLED){
            cout << "[DISPATCHER::Dispatcher] Creating port for: " << portName << endl;
        }
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
        needToRequestJob = true;
        holdIn(AtomicState::active, timeToRequestJobs); 
    } else {
        passivate();	
    }
      cout << "[DISPATCHER::initFunction] Dispatcher atomic initialized" << endl;

    return *this;
}


Model &Dispatcher::externalFunction(const ExternalMessage &msg)
{
    updateTimeVariables(msg);

    if (msg.port() == newJob) {
        this->attendNewJob();
    } else if (msg.port() == jobDone){
        this->attendJobDone(msg);
    } else if(msg.port() == serverStatus){
        this->attendNewStackServerInfo(msg);
    }
    return *this;
}

Model &Dispatcher::internalFunction(const InternalMessage &)
{

    if (jobArrived and this->hasFreeServer()){
        // Si había llegado un job y había servers libres, en la lambda lo mandé, seteo el flag en false
        jobArrived = false;
        // Pongo el server al que le mande el job en -busy- y me guardo el server que está procesando el job
        statusOfServers[serverToDispatch] = SERVER_BUSY;
        jobProcessingServer[this->jobID] = serverToDispatch;

        // Calculo el ID del próximo Job
        this->jobID = this->jobID + Real(1);
    }

    if (needToRequestJob) {
        // Si había necesidad de pedir un job, ya lo pedí en la lambda, ahora lo apago
        needToRequestJob = false;
        requestedJob = true;
    }

    passivate();
    return *this ;
}


Model &Dispatcher::outputFunction(const CollectMessage &msg)
{
    if (jobArrived and this->hasFreeServer()){
        serverToDispatch = getNextServerToDispatch();
	    if (DISPATCHER_DEBUGGING_ENABLED){
	        cout << "[DISPATCHER::outputFunction] Enviando job a procesar:" << this->jobID;
	        cout << " , a server: " << serverToDispatch << endl;
	    }
        sendOutput(msg.time(), *jobsToProcess[serverToDispatch], this->jobID);
    }

    if (needToRequestJob){
        // Si me programé para pedir un job, lo pido
        sendOutput(msg.time(), requestJob, Real(1));
    }


    return *this ;
}

void Dispatcher::attendNewJob(){

    if (not requestedJob){
            MTHROW(MException("Llega un job nuevo pero no fue pedido"));
    } else {
        if (this->hasFreeServer()){
            // Llegó el nuevo job que pedimos y hay servers libres (si no hubiera no lo habría pedido), me programo para enviarlo
            
            // El request del job ya llegó, seteo los flags correspondientes
            requestedJob = false;
            jobArrived = true;
            // Calculo el server al cual mandarle el job y me programo para mandarlo en tiempo Zero
            serverToDispatch = getNextServerToDispatch();

            if (DISPATCHER_DEBUGGING_ENABLED) {
                cout << "[DISPATCHER::attendNewJob] New Job arrived, id assigned: " << this->jobID;
                cout << ", Server to dispatch: " << serverToDispatch << "." << endl;
            }

            // Si luego de mandar este job, quedan servers libres, me programo también para pedir job
            if (numberOfServersFree() > 1){
                needToRequestJob = true;
            }
        } else {
            jobArrived = true;
            requestedJob = false;
            // MTHROW(MException("D! EN LOS TESTS NO DEBERIA LLEGAR ACA!!!"))
        }

        holdIn(AtomicState::active, timeToDispatchJobs);
    }
}

void Dispatcher::attendJobDone(const ExternalMessage &msg){
    // Un servidor está informando que termino el job, por lo tanto ahora está libre.
    // Lo libero y me programo para pedir otro, en caso de no haberlo pedido ya
    Real jobIDDone = Real::from_value(msg.value());
     if (DISPATCHER_DEBUGGING_ENABLED)
        cout << "[DISPATCHER::attendJobDone] Done job: " << jobIDDone << endl;


    if (jobProcessingServer.count(jobIDDone) == 0){
        MTHROW(MException("LLegó job hecho pero el dispatcher no sabe quien lo estaba procesando"));
    } else {
        int serverThatProcessedTheJob = jobProcessingServer[jobIDDone];
        jobProcessingServer.erase(jobIDDone);
        // Me fijo si el servidor estaba marcado para apagar cuando termine
        if (markAsOffWhenDone.count(serverThatProcessedTheJob) > 0){
            statusOfServers[serverThatProcessedTheJob] = SERVER_OFF;
            markAsOffWhenDone.erase(serverThatProcessedTheJob);
            if (serverToDispatch == serverThatProcessedTheJob){
                serverToDispatch = getNextServerToDispatch();
                if (serverToDispatch == -1){ needToRequestJob = false; }
            }
        } else {
            // Como no estaba marcado para apagar, lo paso a FREE
            statusOfServers[serverThatProcessedTheJob] = SERVER_FREE;
        }


        if (requestedJob){
            // si ya pedí un job (y todavía no llego) me pasivo
            passivate();
        } else {
            // si no, me programo para pedir otro
            needToRequestJob = true;
            holdIn(AtomicState::active, timeToRequestJobs);
        }

    }
}

void Dispatcher::attendNewStackServerInfo(const ExternalMessage &msg){

    if (DISPATCHER_DEBUGGING_ENABLED){
	   cout << "[DISPATCHER] Llego informacion de servidor " << *msg.value() << endl;
    }
    Tuple<Real> newServerInfo = Tuple<Real>::from_value(msg.value());

    int server = (int) newServerInfo[0].value();
    string statusOfServer = statusOfServers[server];
    Real powerSignal = newServerInfo[1];

    if ( (bool) (powerSignal == POWER_OFF_SIGNAL) and statusOfServer == SERVER_OFF){
        MTHROW(MException("D! Mensaje de servidor apagado a servidor ya apagado"))
    } else if ( (bool) (powerSignal == POWER_ON_SIGNAL) and statusOfServer == SERVER_BUSY){
        MTHROW(MException("D! Mensaje de servidor encendido a servidor ya en busy"))
    } else if ( (bool) (powerSignal == POWER_ON_SIGNAL) and statusOfServer == SERVER_FREE){
        MTHROW(MException("D! Mensaje de servidor encendido a servidor ya encendido"))
    } else if ( (bool) (powerSignal == POWER_OFF_SIGNAL) and statusOfServer == SERVER_FREE){
        // Marco como apagado el servidor
        statusOfServers[server] = SERVER_OFF;
        passivate();
    } else if ( (bool) (powerSignal == POWER_ON_SIGNAL) and statusOfServer == SERVER_OFF){
        // Marco como prendido el servidor
        statusOfServers[server] = SERVER_FREE;
        // Se prendió un servidor, si había un job pendiente, lo mando.
        if (jobArrived){
            serverToDispatch = getNextServerToDispatch();
            // Si luego de mandar este job, quedan servers libres, me programo también para pedir job
            if (numberOfServersFree() > 1){
                needToRequestJob = true;
            }

            holdIn(AtomicState::active, timeToDispatchJobs);
        } else if (not requestedJob){
            // Ahora hay al menos un servidor libre y no había pedidos, tengo que pedir un job
            needToRequestJob = true;
            holdIn(AtomicState::active, timeToRequestJobs);
        } else {
            passivate();
        }

    } else if ( (bool) (powerSignal == POWER_OFF_SIGNAL) and statusOfServer == SERVER_BUSY){
        // Me programo para marcar el servidor como apagado una vez que termina el job
        if (markAsOffWhenDone.count(server) > 0){
            MTHROW(MException("D! Ya estaba marcado para apagar!"))
        } else {
            markAsOffWhenDone.insert(server);
        }
        passivate();
    } else {
        bool signalIsPowerOn = powerSignal == POWER_ON_SIGNAL;
        cerr << "Power Signal: " << powerSignal << endl;
        cerr << "Signal equal to on: " << signalIsPowerOn << endl;
        cerr << "Server number: " << server << endl;
        cerr << "Status of server: " << statusOfServer << endl;
        MTHROW(MException("D! Estado inválido cuando llega nueva información de servidor! Raro! "))

    }
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

int Dispatcher::numberOfServersFree(){
    int freeServers = 0;
    for (int i = 0; i < this->numberOfServers; i++){
        if (statusOfServers[i] == SERVER_FREE) {
            freeServers++;
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

