-- Llega un nuevo job y el dispatcher lo envía al server 3

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
server3: free





