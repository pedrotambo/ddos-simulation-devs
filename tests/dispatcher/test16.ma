[top]
components: a_dispatcher@Dispatcher
out: requestJob server0 server1 server2 server3 server4
in: newJob jobDone serverStackInfo
link: newJob newJob@a_dispatcher
link: jobDone jobDone@a_dispatcher
link: serverStackInfo serverStackInfo@a_dispatcher
link: requestJob@a_dispatcher requestJob
link: server0@a_dispatcher server0
link: server1@a_dispatcher server1
link: server2@a_dispatcher server2
link: server3@a_dispatcher server3
link: server4@a_dispatcher server4


[a_dispatcher]
numberOfServers: 5
