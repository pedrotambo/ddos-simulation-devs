#include <string>
#include <random>


#include "attacker.h"

using namespace std;

const string DEFAULT_ATTACK_EVENTS_FILE = "attack-data/ataque.txt";

Attacker::Attacker(const string &name) : 
    Atomic(name),
	attack(addOutputPort("attack"))
{

	string attackEventsPath;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "file" ) ){
		attackEventsPath = ParallelMainSimulator::Instance().getParameter( description(),"file" );
	} else {
		attackEventsPath = DEFAULT_ATTACK_EVENTS_FILE;	
	}

	attackEventsFile.open(attackEventsPath);


	if (attackEventsFile.is_open()){
	} else {
		MTHROW(MException("Error cargando archivo de ataque!"))
	}


	cout << "Attacker atomic succesfully created" << endl;



}


Model &Attacker::initFunction()
{
	VTime nextJobTime = this->getNextJobTime();
	
	holdIn(AtomicState::active, nextJobTime); 

	return *this;
}

VTime Attacker::getNextJobTime(){
        string line;
        getline(attackEventsFile,line);
        string time;
        string value;
        istringstream lineStream(line);
        lineStream >> time;
        lineStream >> value;

        if (time != "END"){
        	return VTime(time);
        } else {
        	return VTime::Inf;
        }
}




Model &Attacker::externalFunction(const ExternalMessage &msg)
{
	// Real messageValue = Real::from_value(msg.value());
	// updateTimeVariables(msg);

	MTHROW(MException("No debería pasar nunca esto"))
	return *this;
}



void Attacker::updateTimeVariables(const ExternalMessage &msg){
	timeLeft = nextChange();
	elapsed = msg.time() - lastChange();
	sigma = elapsed + timeLeft;
}

Model &Attacker::internalFunction(const InternalMessage &)
{
	
	VTime nextJobTime = this->getNextJobTime();

	if (nextJobTime == VTime::Inf){
		attackEventsFile.close();
	}


	holdIn(AtomicState::active, nextJobTime); 

	return *this ;
}


Model &Attacker::outputFunction(const CollectMessage &msg)
{
	sendOutput(msg.time(), attack, 1.0);


	return *this ;
}
