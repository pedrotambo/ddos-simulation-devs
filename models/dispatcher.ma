[top]
components : dispatcher@dispatcher
in : job serverStackInfo
out : jobToProcess
link : job job@dispatcher
link : serverStackInfo serverStackInfo@dispatcher
link : jobToProcess@dispatcher jobToProcess

[dispatcher]
in : job powerSignal
out : jobToProcess
numberOfServers : 10
server5: busy
server9: off





