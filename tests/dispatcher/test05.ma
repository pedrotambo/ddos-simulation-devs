-- El dispatcher empieza con 2 servers libres. Llegan 2 jobs, los envía. Se termina 1, queda uno libre. Llega 1 job, lo envía al server 2, no quedan libres. 
-- Se termina el del 3, queda el 3 libre. Llega uno, y se manda 

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
server0: busy
server1: off
server2: free
server3: free
server4: off





