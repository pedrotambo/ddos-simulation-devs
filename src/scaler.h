
#ifndef _AutoScaler_H_
#define _AutoScaler_H_

#include <vector>
#include <queue>
#include<map>

#include "atomic.h"
#include "VTime.h"

#define AUTO_SCALER_ATOMIC_MODEL_NAME "AutoScaler"

class AutoScaler : public Atomic {
/*
@PringlesModelMetadata
name: AutoScaler
input_ports: queueLoad, serverResponse
output_ports: serverStatus
*/

    public:
        typedef std::list<value_ptr> ElementList;

        AutoScaler(const string &name = AUTO_SCALER_ATOMIC_MODEL_NAME );
        virtual string className() const {return AUTO_SCALER_ATOMIC_MODEL_NAME;}

    protected:
        Model &initFunction();
        Model &externalFunction( const ExternalMessage & );
        Model &internalFunction( const InternalMessage & );
        Model &outputFunction( const CollectMessage & );

    private:
        // Lifetime programmed since the last state transition to the next planned internal transition.
        VTime sigma;
        // Time elapsed since the last state transition until now
        VTime elapsed;	

        // Time remaining to complete the last programmed Lifetime
        VTime timeLeft;
        
        VTime previous_sigma; // Para reestablecer después de forzar transiciones internas
        
        // internal variables
        
        int idle_updates_left; // Contador para saber cuando vuelvo a considerar prender un servidor
        
        uint server_farm_size; // Cantidad de servers
        
        uint load_updates_to_break_idle; // Cuantos updates del factor de carga espero hasta tomar una decision
        
        bool signaling_server;

        float exponential_weight; // alfa para el moving average
        float load_moving_avg; // moving average
        float load_lower_bound; // cota por debvajo de la cual apago un server
        float load_upper_bound; // cota a partir de la cual prendo un server
        
        bool has_server_update;
        Tuple<Real> server_update; // Informacion para enviar al dispatcher

        Port& queueLoad;
        Port& serverResponse;
        Port& serverStatus;

        std::map<int, Port*> servers;
        std::map<int, string> server_status;
        
        int getPoweredOffServer();
        int getPoweredOnServer();
        int runningServers();
        void updateLoadFactor(double new_val);
        void updateServerStatus(const Tuple<Real>& server_update);
        bool shouldPowerOffServer();
        bool shouldPowerOnServer();
};

#endif