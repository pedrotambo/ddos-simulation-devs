#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include "VTime.h"


// SERVER CONSTANTS
const Real POWER_ON_SIGNAL = Real(1); 
const Real POWER_OFF_SIGNAL = Real(0); 
const VTime DEFAULT_SETUP_TIME = VTime("0:0:5:0");
const double DEFAULT_MEAN = 1.0;
const double SERVER_READY_MESSAGE = 1.0;
const double JOB_DONE_MESSAGE = 0.0;

#endif