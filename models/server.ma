[top]
components : server@server attacker@attacker
in : job powerSignal
out : done ready
link : job job@server
link : powerSignal powerSignal@server
link : done@server done
link : ready@server ready
link : attack@attacker job@server

[server]
in : job powerSignal
out : done ready
mean : 0.0000001
distribution : exponential
setupTime : 00:00:10:000
initialStatus : free

[attacker]
out : attack
file : attack-data/ataque.txt


