%Si bajo la actividad del servidor y tengo +1 prendido
%mando a apagar alguno

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
loadLowerBound : 0.49
loadUpperBound : 0.5
loadUpdatesToBreakIdle : 3
server0 : free