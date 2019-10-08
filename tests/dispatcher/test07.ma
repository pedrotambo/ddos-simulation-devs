-- El server arranca con 3 servers frees.
-- Se piden y llegan 2 jobs, se despachan al server 1 y 2. Queda el 4 free, se pide otro job.
-- Se termina el job en el server 1, no se pide nada porque ya se había pedido.
-- Llega el job y se lo mandar al 1 (menor id que el), se pide otro job (4 free).
-- Se termina el job en el server 2, no se pide nada porque ya se había pedido.
-- Llega el job y se lo manda al 2, queda el 4 free, se pide otro job.
-- Llega el job y se lo manda al 4, no se pide otro job porque están todos ocupados.


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
server1: free
server2: free
server3: off
server4: free