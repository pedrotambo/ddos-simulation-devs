#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include "VTime.h"
#include <string>

const bool QUEUE_DEBUGGING_ENABLED = false;
const bool SERVER_DEBUGGING_ENABLED = false;
const bool DISPATCHER_DEBUGGING_ENABLED = false;
const bool SCALER_DEBUGGING_ENABLED = true;

// SERVER CONSTANTS
const std::string SERVER_FREE = "free";
const std::string SERVER_OFF = "off";
const std::string SERVER_BUSY = "busy";
const std::string DEFAULT_SERVER_STATE = SERVER_OFF;
const Real POWER_ON_SIGNAL = Real(1); 
const Real POWER_OFF_SIGNAL = Real(0); 
const VTime DEFAULT_SETUP_TIME = VTime("0:0:5:0");
const double DEFAULT_MEAN = 1.0;
const double SERVER_READY_MESSAGE = 1.0;
const double SERVER_OFF_MESSAGE = 1.0;
const double JOB_DONE_MESSAGE = 0.0;

// AUTOSCALER CONSTANTS
const double LOAD_LOWER_BOUND = 0.2;
const double LOAD_UPPER_BOUND = 0.9;
const int LOAD_UPDATES_TO_BREAK_IDLE = 5; //Cuantos updates del factor de carga espero para prender/apagar otro servidor


// ATTACKER CONSTANTS
const string DEFAULT_ATTACK_EVENTS_FILE = "attack-data/ataque.txt";

#endif