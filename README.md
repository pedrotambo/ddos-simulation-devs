# DDoS Simulation using CD++ DEVS
In this project we are going to simulate a DDoS attack behind a system with autoscaling using DEVS formalism and analyze performance in different scenarios. CD++ DEVS is a simulation tool for simulating DEVS (discrete event syste specifiction).

## Build
To build the source code ensure that the CDPP_PATH variable in src/Makefile points to a CD++ binary. Then run:
```
cd src && make
```
## Tests
Once the project is built, you can run the tests:
```
cd tests && sh run_tests.sh
```

# Experimentation
In the root of the project we can see different types of notebooks:
- Testing notebooks:
	- Dispatcher/Server coupled: a top model with only one atomic for test purposes.
	- Server-Dispatcher: a model with the interaction of a server and a dispatcher.
	- Queue-Server-Dispatcher: a model with the interaction of a queue, server and a dispatcher.
	- Queue-Server-Dispatcher-Attacker: a model with the interaction of a queue, server, dispatcher and an attacker.

- Dynamics ports:
	- Puertos Dinámicos: a notebook with the explanation of how to implement dynamic ports

- Experiments:
	- Experimentación con y sin scaler: an experiment using the system with and without the auto-scaler against the normal traffic and the attacker.
	- Alterando lambda: an experiment varying the processing power of the servers to see how it affects the queue load

### Installation
To preview the notebooks you will need to have python 3.7. If it's not the default version you can provide the path to the binary. Then run:
```
sh setup.sh [optional: python3.7 binary path]
source venv/bin/activate
jupyter notebook
```


### Simulation Results
When running notebooks, simulation results will be stored in /sim_results directory.

### Attack Data
Data about attacks is located within /attack-data directory.

# Final Report
Inside /report directory there is the jupyter notebook 'Informe.ipynb' with all the information about the models, experimentation result analisys and conclusions.
