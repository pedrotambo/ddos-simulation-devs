#include <string>
#include <random>


#include "attacker.h"

using namespace std;



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
	
	passivate();
	holdIn(AtomicState::active, nextJobTime);
	cout << "[ATTACKER::initFunction] Attacker atomic initialized" << endl;

	return *this;
}


VTime Attacker::getNextJobTime(){
        string line;
        getline(attackEventsFile,line);
        if (attackEventsFile.bad()){
        	cout << "END OF FILE" << endl;
        	return VTime::Inf;
        }
        string time;
        string value;
        istringstream lineStream(line);
        lineStream >> time;
        lineStream >> value;

        if (time != "END" and time != ""){
        	return VTime(time);
        } else {
        	cout << "Last item!" << endl;
        	return VTime::Inf;
        }
}



Model &Attacker::externalFunction(const ExternalMessage &msg)
{
	MTHROW(MException("No debería pasar nunca esto"))
	return *this;
}


Model &Attacker::internalFunction(const InternalMessage &)
{
	
	VTime nextJobTime = this->getNextJobTime();

	if (nextJobTime == VTime::Inf){
		cout << "Closing file.." << endl;
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

void Attacker::updateTimeVariables(const ExternalMessage &msg){
	timeLeft = nextChange();
	elapsed = msg.time() - lastChange();
	sigma = elapsed + timeLeft;
}
