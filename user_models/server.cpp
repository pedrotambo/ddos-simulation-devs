#include <string>
#include <random>

#include "message.h"
#include "parsimu.h"
#include "distri.h"

#include "Server.h"

using namespace std;


Server::Server(const string &name) : 
    Atomic(name),
	job(addInputPort("job")),
	powerSignal(addInputPort("powerSignal")),
	done(addOutputPort("done")),
	ready(addOutputPort("ready")),
	mean(1.0),
	initialStatus("off"),
	setupTime("0:0:5:0")
{
	if( ParallelMainSimulator::Instance().existsParameter( description(), "mean" ) )
		mean = stof( ParallelMainSimulator::Instance().getParameter( description(), "mean" ) );

	if( ParallelMainSimulator::Instance().existsParameter( description(), "initialStatus" ) )
		status = ParallelMainSimulator::Instance().getParameter( description(), "initialStatus" );

	distribution = Distribution::create("exponential");

	MASSERT( distribution ) ;
	for ( register int i = 0; i < distribution->varCount(); i++ )
	{
		string parameter( ParallelMainSimulator::Instance().getParameter( description(), distribution->getVar( i ) ) ) ;
		distribution->setVar( i, str2Value( parameter ) ) ;
	}
	cout << "Server atomic succesfully created" << endl;

}


Model &Server::initFunction()
{
	passivate()
	// frequency
  	cout << "Server atomic initialized" << endl;

	return *this;
}


Model &Server::externalFunction(const ExternalMessage &msg)
{
	sigma = sigma - elapsed;
	if (msg.port() == job) {
		if(status == "off") {
			cout << "Llego mensaje a servidor apagado" << endl;
			throw "Llego mensaje a servidor apagado";
		} else if (status == "busy") {
			cout << "Llego mensaje a servidor ocupado" << endl;
			throw "Llego mensaje a servidor ocupado";		
		} else if (status == "free"){
			VTime job_processing_time = VTime( static_cast< float >( fabs(distribution().get() ) ) );
			status = "busy";
			holdIn(AtomicState::active, job_processing_time) ;
		} else {
			throw "estado invalido";
		}
	} else if (msg.port() == powerSignal and msg.value() == 0) {
		if(status == "off") {
			//TODO: resolver este caso
			throw "Apagado a servidor apagado"
		} else if (status == "busy") {
			turnOffAfterCompletion = true;
		} else if (status == "free"){
			status = "off";
			passivate();
		} else {
			throw "estado invalido";
		}
	} else if (msg.port() == powerSignal and msg.value() == 1) {
		if(status == "off") {
			if (poweringOn) {
				throw "Doble mensaje de prendido";
			}
			poweringOn = true;
			holdIn(AtomicState::active, setupTime);
		} else if (status == "busy") {
			//TODO: resolver este caso
			throw "Prendido a servidor busy";
		} else if (status == "free"){
			//TODO: resolver este caso
			throw "Prendido a servidor free";
		} else {
			throw "estado invalido";
		}
	}
	return *this;
}


Model &Server::internalFunction(const InternalMessage &)
{

	int i; // auxiliary index
	std::vector<double> cumprob; // cumulative probability
	auto random_float = this->dist(this->rng); // flip a coin

	cumprob.resize(this->N);

	// we rely on the discrete inverse transform method to test the markov chain
	cumprob[0] = this->P[this->state-1][0];
	for (i = 1; i < this->N; i++){
		cumprob[i] = this->P[this->state-1][i] + cumprob[i-1];
	}

	i = 0;
	while(random_float >= cumprob[i]){
		i++;
	}

	this->state = i+1;

	// frequency
	this->stateCount[this->state-1]++,
	this->totalCount++;

	holdIn(AtomicState::active, this->step_time);

	return *this ;
}


Model &Server::outputFunction(const CollectMessage &msg)
{

	Tuple<Real> outValue{1,0,0};

	sendOutput(msg.time(), currentState_o, Tuple<Real>({this->state,this->stateCount[this->state-1],this->totalCount}));
	// cout << msg.time() << " state == " << std::to_string(this->state) << endl;

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