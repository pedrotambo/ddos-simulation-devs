%Si el server esta sobrecargado mando a prender otro

[top]
out : out
in : queueLoad serverResponse
components : as@AutoScaler
link : queueLoad queueLoad@as
link : serverResponse serverResponse@as 
link : serverStatus@as out

[as]
numberOfServers : 2
exponentialWeight : 0.6 
loadLowerBound : 0.4
loadUpperBound : 0.5
loadUpdatesToBreakIdle : 5
server0 : on