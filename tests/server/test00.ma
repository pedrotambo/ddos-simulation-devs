-- server empieza apagado y le llega job tira excepcion

[top]
components : server@server
in : job powerSignal
out : done ready
link : job job@server
link : powerSignal powerSignal@server
link : done@server done
link : ready@server ready

[server]
in : job powerSignal
out : done ready
mean : 10.0
distribution : exponential
setupTime : 00:00:10:000
initialStatus : off





