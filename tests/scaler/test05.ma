%Nunca apago si tengo un solo servidor prendido

[top]
out : out
in : queueLoad serverResponse
components : as@AutoScaler
link : queueLoad queueLoad@as
link : serverResponse serverResponse@as 
link : serverStatus@as out

[as]
numberOfServers : 1
exponentialWeight : 0.6 
loadLowerBound : 0.4
loadUpperBound : 0.5
loadUpdatesToBreakIdle : 5
server0 : on