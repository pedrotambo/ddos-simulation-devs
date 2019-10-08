
-- El dispatcher pide un job, le llega lo envía por el servidor 2, y pide otro porque le sigue quedando otro más libre. 

-- Le llega que marque apagado el servidor 2, que está en busy, espera a que lo termine.
-- Le llega que el servidor 2 terminó el job, lo marca como apagado.
-- Le llega que marque como prendido el servidor 2, lo marca como prendido.
-- Le llega el job que había pedido, lo manda al servidor 2. Pide otro porque el 3 todavía esta libre.


[top]
components : dispatcher@dispatcher
in : newJob jobDone serverStackInfo
out : requestJob server0 server1 server2 server3 server4
link : newJob newJob@dispatcher
link : jobDone jobDone@dispatcher 
link : serverStackInfo serverStackInfo@dispatcher

link : requestJob@dispatcher requestJob
link : server0@dispatcher server0
link : server1@dispatcher server1
link : server2@dispatcher server2
link : server3@dispatcher server3
link : server4@dispatcher server4


[dispatcher]
in : newJob jobDone serverStackInfo
out : requestJob server0 server1 server2 server3 server4
numberOfServers : 5
-- by default servers are off until they express initial status, but for testing purposes, initial server status can be set
server0: off
server1: off
server2: free
server3: free
server4: off
