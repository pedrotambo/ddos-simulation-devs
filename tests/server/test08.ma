[top]
components: a_server@Server
out: done ready
in: job powerSignal
link: job job@a_server
link: powerSignal powerSignal@a_server
link: done@a_server done
link: ready@a_server ready


[a_server]
initialStatus: free
mean: 10.0
setupTime: 0:0:10:0




--[top]
--components : server@server
--in : job powerSignal
--out : done ready
--link : job job@server
--link : powerSignal powerSignal@server
--link : done@server done
--link : ready@server ready

--[server]
--in : job powerSignal
--out : done ready
--mean : 10.0
--distribution : exponential
--setupTime : 00:00:10:000
--initialStatus : free





