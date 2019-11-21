# ddos-simulation-devs
In this project we are going to simulate a DDoS attack behind a system with autoscaling using DEVS formalism and analyze performance in different scenarios


# Jupyter-Notebooks

In the root of the project we can see different types of notebooks:
	- Testing notebooks:
		. Dispatcher/Server coupled: a top model with only one atomic for test purposes.
		. Server-Dispatcher: a model with the interaction of a server and a dispatcher.
		. Queue-Server-Dispatcher: a model with the interaction of a queue, server and a dispatcher.
		. Queue-Server-Dispatcher-Attacker: a model with the interaction of a queue, server, dispatcher and an attacker.

	- Dynamics ports:
		. Puertos Dinámicos: a notebook with the explanation of how to implement dynamic ports

	- Experiments:
		. Experimentación con y sin scaler: an experiment using the system with and without the auto-scaler against the normal traffic and the attacker.
		. Alterando lambda: an experiment varying the processing power of the servers to see how it affects the queue load


# /src: source code of the atomics for cd++.

# /sim_results: directory for results of simulations.

# /tests: tests that were done as the models were created.

# /attack-data: directory with files for the attacker, where its contents reflect the time difference between each requests

# Compilation: please provide the source code directory of the CD++ (CDPP_PATH) in the file src/Makefile, if necessary. Run make inside src directory (make sure CD++ is already compiled).

# /report: Content for the report of the project. Open 'Informe.ipynb' for visualization.