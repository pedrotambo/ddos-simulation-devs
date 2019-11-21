
#ifndef _ATTACKER_H_
#define _ATTACKER_H_

#include <random>
#include <math.h>
#include <iostream>
#include <fstream>
#include "atomic.h"
#include "VTime.h"
#include "distri.h"
#include "message.h"
#include "parsimu.h"
#include "constants.h"
#include "except.h"


#define ATTACKER_ATOMIC_MODEL_NAME "Attacker"


class Attacker : public Atomic {
/*
@PringlesModelMetadata
name: Attacker
output_ports: attack
*/

  public:
    Attacker(const string &name = ATTACKER_ATOMIC_MODEL_NAME );
    virtual string className() const {return ATTACKER_ATOMIC_MODEL_NAME;}

  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );

    VTime getNextJobTime();


  private:
    VTime nextJobTime;
    // Out Ports
    Port &attack;

    // Lifetime programmed since the last state transition to the next planned internal transition.
    VTime sigma;	
    // Time elapsed since the last state transition until now
    VTime elapsed;
    // Time remaining to complete the last programmed Lifetime
    VTime timeLeft;

    ifstream attackEventsFile;

    void updateTimeVariables(const ExternalMessage &);


};

#endif


