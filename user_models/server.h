
#ifndef _DTMC_H_
#define _DTMC_H_

#include <random>

#include "atomic.h"
#include "VTime.h"

#define ATOMIC_MODEL_NAME "DTMC"

class DTMC : public Atomic {
/*
@ModelMetadata
name: DTMC
output_ports: currentState_o
*/

  public:
    DTMC(const string &name = ATOMIC_MODEL_NAME );
    virtual string className() const {return ATOMIC_MODEL_NAME;}

  protected:
    Model &initFunction();
    Model &externalFunction( const ExternalMessage & );
    Model &internalFunction( const InternalMessage & );
    Model &outputFunction( const CollectMessage & );

  private:
    // const Port &numProdClient_i;I
    Port &currentState_o;
    
    // Lifetime programmed since the last state transition to the next planned internal transition.
    VTime sigma;
	
    // Time elapsed since the last state transition until now
    VTime elapsed;
	
    // Time remaining to complete the last programmed Lifetime
    VTime timeLeft;

    // internal variables
    int state;
    int initial;
    VTime step_time;

    int N; // number of states
    float s, r; // transition probabilities

    int totalCount;

    std::vector<std::vector<double>> P; // matrix = vector of vectors

    std::vector<int> stateCount; // matrix = vector of vectors

    std::uniform_real_distribution<> dist;
    std::uniform_int_distribution<int> dist_initial;
    std::mt19937 rng;

    double get_param(const string &);

};

#endif