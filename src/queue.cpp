#include <string>

#include "message.h"
#include "parsimu.h"
#include "distri.h"

#include "queue.h"

using namespace std;

ServerQueue::ServerQueue(const string &name) :
    Atomic(name), 
    in(addInputPort("in")), 
    emit(addInputPort("emit")),
    out(addOutputPort("out")), 
    discarded(addOutputPort("discarded")),
    queueLoad(addOutputPort("queueLoad"))
{
    currentSizeFrequency = VTime("00:00:01:000");
    size = 50;
    emition_pending = false;
    previous_sigma = VTime::Zero;

    if( ParallelMainSimulator::Instance().existsParameter( description(), "size" ) )
        size = stoi( ParallelMainSimulator::Instance().getParameter( description(), "size" ) );

    if( ParallelMainSimulator::Instance().existsParameter( description(), "currentSizeFrequency" ) ){
        string time( ParallelMainSimulator::Instance().getParameter( description(), "currentSizeFrequency" ) ) ;
        if( time != "" ) currentSizeFrequency = VTime(time) ;    
    }
    
    

    cout << "[QUEUE]ServerQueue atomic succesfully created" << endl;
}


Model &ServerQueue::initFunction()
{
    cout << "[QUEUE]Initializing ServerQueue" << endl;
    dropped_jobs.erase(dropped_jobs.begin(), dropped_jobs.end());
    queue.erase(queue.begin(), queue.end());

    holdIn(AtomicState::active, currentSizeFrequency);
    cout << "[QUEUE]ServerQueue atomic initialized" << endl;

    return *this;
}


Model &ServerQueue::externalFunction(const ExternalMessage &msg)
{
    sigma = nextChange();

    if (msg.port() == in) {
        auto job_id = msg.value();
        
        if (queue.size() == size) {
            if (QUEUE_DEBUGGING_ENABLED)
                cout << "[QUEUE] Llego trabajo llena" << endl;
            previous_sigma = sigma;
            dropped_jobs.push_back(job_id);
            holdIn(AtomicState::active, VTime::Zero);
        } else {
            if (QUEUE_DEBUGGING_ENABLED)
                cout << "[QUEUE] Agrego trabajo " << *msg.value() << " " << msg.time() << endl;
            queue.push_back(job_id);
            if (emition_pending) { //Me había llegado un pedido con la cola vacía
                holdIn(AtomicState::active, VTime::Zero);
            }
        }
    } else if (msg.port() == emit) {
        if (QUEUE_DEBUGGING_ENABLED) 
            cout << "[QUEUE]Llego pedido" << endl;
        emition_pending = true;
        if (!queue.empty()) {
            holdIn(AtomicState::active, VTime::Zero);
            previous_sigma = sigma;
        }
    }
    return *this;
}


Model &ServerQueue::internalFunction(const InternalMessage &)
{
    if (previous_sigma != VTime::Zero) {
        holdIn(AtomicState::active, previous_sigma);
        previous_sigma = VTime::Zero;
    } else {
        
        holdIn(AtomicState::active, currentSizeFrequency);
    }

    if(!dropped_jobs.empty()) {
        dropped_jobs.erase(dropped_jobs.begin(), dropped_jobs.end());
    }

    if (!queue.empty() && emition_pending) {
        emition_pending = false;
        queue.pop_front();
    }

    return *this;
}


Model &ServerQueue::outputFunction(const CollectMessage &msg)
{
    if (previous_sigma == VTime::Zero) { //Tengo que reportar el tamaño
        if (QUEUE_DEBUGGING_ENABLED)
            cout << "[QUEUE]Mando factor de carga " << queue.size()/(float)size << endl;
        sendOutput(msg.time(), queueLoad, queue.size()/(float)size);
    }

    if (!dropped_jobs.empty()) { //Envio los trabajos droppeados 
        for (auto job_id : dropped_jobs) {
            if (QUEUE_DEBUGGING_ENABLED)
                cout << "[QUEUE]Dropeo tarea " << *job_id << endl;
            sendOutput(msg.time(), discarded, *job_id);
        }
    }

    if (!queue.empty() && emition_pending) { //Si tenia que mandar algo lo mando y lo saco de la cola
        if (QUEUE_DEBUGGING_ENABLED)
            cout << "[QUEUE]Mando trabajo " << *queue.front() << endl;
        sendOutput(msg.time(), out, *queue.front());
    }

    return *this;
}