[top]
components: gen@Generator queue@ServerQueue dispatcher@Dispatcher as@AutoScaler server0@Server server1@Server
out: processedJob droppedJob
in: 
link: out@gen in@queue
link: out@queue newJob@dispatcher
link: current_size@queue queueLoad@as
link: requestJob@dispatcher emit@queue
link: server0@dispatcher job@server0
link: server1@dispatcher job@server1
link: server0@as powerSignal@server0
link: server1@as powerSignal@server1
link: serverStatus@as serverStackInfo@dispatcher
link: done@server0 jobDone@dispatcher
link: done@server1 jobDone@dispatcher
link: ready@server0 serverResponse@as
link: ready@server1 serverResponse@as
link: discarded@queue droppedJob
link: done@server0 processedJob
link: done@server1 processedJob


[gen]
distribution: normal
mean: 0.01
deviation: 1
initial: 0
increment: 1


[queue]
size: 100
currentSizeFrequency: 00:00:05:00


[dispatcher]
numberOfServers: 2
server0: free


[as]
numberOfServers: 2
exponentialWeight: 0.6
loadLowerBound: 0.3
loadUpperBound: 0.8
loadUpdatesToBreakIdle: 10
server0: free


[server0]
initialStatus: free
distribution: exponential
mean: 1.0
setupTime: 00:00:10:00


[server1]
initialStatus: off
distribution: exponential
mean: 1.0
setupTime: 00:00:10:00
