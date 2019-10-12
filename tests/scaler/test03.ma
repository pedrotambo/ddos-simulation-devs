%Si despues de prender un server no baja
%la carga prendo otro

[top]
out : out
in : queueLoad serverResponse
components : as@AutoScaler
link : queueLoad queueLoad@as
link : serverResponse serverResponse@as 
link : serverStatus@as out

[as]
numberOfServers : 3
exponentialWeight : 0.6 
loadLowerBound : 0.4
loadUpperBound : 0.5
loadUpdatesToBreakIdle : 3
server0 : on