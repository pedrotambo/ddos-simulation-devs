#include <string>

#include "message.h"
#include "parsimu.h"
#include "distri.h"

#include "queue.h"

using namespace std;

ServerQueue::ServerQueue(const string &name) :
    Atomic(name), in(addInputPort("in")), emit(addInputPort("emit")),
    out(addOutputPort("out")), discarded(addOutputPort("discarded")),
    current_size(addOutputPort("current_size"))
{
    current_size_frequency = "00:00:01:000";
    size = 50;
    emition_pending = false;
    previous_sigma = VTime::Zero;

    if( ParallelMainSimulator::Instance().existsParameter( description(), "size" ) )
        size = stoi( ParallelMainSimulator::Instance().getParameter( description(), "size" ) );

    string time( ParallelMainSimulator::Instance().getParameter( description(), "current_size_frequency" ) ) ;
    if( time != "" ) current_size_frequency = time ;

    cout << "ServerQueue atomic succesfully created" << endl;
}


Model &ServerQueue::initFunction()
{
    cout << "Initializing ServerQueue" << endl;
    dropped_jobs.erase(dropped_jobs.begin(), dropped_jobs.end());
    queue.erase(queue.begin(), queue.end());

    holdIn(AtomicState::active, current_size_frequency);
    cout << "ServerQueue atomic initialized" << endl;

    return *this;
}


Model &ServerQueue::externalFunction(const ExternalMessage &msg)
{
    sigma = sigma - elapsed;
    if (msg.port() == in) {
        auto job_id = msg.value();
        if (queue.size() == size) {
            cout << "Llego trabajo llena" << endl;
            if (dropped_jobs.size() == 0) {
                holdIn(AtomicState::active, VTime::Zero);
            }
            dropped_jobs.push_back(job_id);
            previous_sigma = sigma;
        } else {
            cout << "Agrego trabajo" << endl;
            queue.push_back(job_id);
            if (emition_pending) {
                holdIn(AtomicState::active, VTime::Zero);
            }
        }
    } else if (msg.port() == emit) {
        cout << "Llego pedido" << endl;
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
        holdIn(AtomicState::active, current_size_frequency);
    }

    return *this;
}


Model &ServerQueue::outputFunction(const CollectMessage &msg)
{
    if (previous_sigma == VTime::Zero) {
        //Tengo que reportar el tamaño
        cout << "Mando tamaño " << queue.size() << endl;
        sendOutput(msg.time(), current_size, queue.size());
    }

    if (!dropped_jobs.empty()) {
        //Envio los trabajos droppeados 
        for (auto job_id : dropped_jobs) {
            cout << "Dropeo tarea " << *job_id << endl;
            sendOutput(msg.time(), discarded, *job_id);
        }
        dropped_jobs.erase(dropped_jobs.begin(), dropped_jobs.end());
    }

    if (!queue.empty() && emition_pending) {
        //Si tenia que mandar algo lo mando y lo saco de la cola
        emition_pending = false;
        cout << "Mando trabajo " << *queue.front() << endl;
        sendOutput(msg.time(), out, *queue.front());
        queue.pop_front();
    }

    return *this;
}