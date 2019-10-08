
-- El dispatcher pide un job, porque tiene un servidor sólo libre, el 2.
-- Le llega que marque como apagado su único servidor prendido, el 2.
-- Le llega el job que había pedido, todavía no lo manda porque se le apagaron los servidores.
-- Le llega que se prendió el servidor 3, entonces lo manda por el servidor 3, que queda en busy.
-- Le llega que se prendió el servidor 1, pide un nuevo job, que llega y lo manda por el 1

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
server3: off
server4: off
