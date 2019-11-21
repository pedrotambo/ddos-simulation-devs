[top]
components : dispatcher@dispatcher server0@server server1@server server2@server server3@server server4@server 
in : newJob jobDone serverStackInfo
out : requestJob server0 server1 server2 server3 server4
link : newJob newJob@dispatcher
link : jobDone jobDone@dispatcher 
link : serverStackInfo serverStackInfo@dispatcher
link : requestJob@dispatcher requestJob
link : server0@dispatcher job@server0
link : server1@dispatcher job@server1
link : server2@dispatcher job@server2
link : server3@dispatcher job@server3
link : server4@dispatcher job@server4

[dispatcher]
in : newJob jobDone serverStackInfo
out : requestJob server0 server1 server2 server3 server4
numberOfServers : 5

server0: free
server1: free
server2: off
server3: free
server4: free
