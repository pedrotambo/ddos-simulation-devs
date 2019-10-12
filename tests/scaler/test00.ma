% Cuando llega el factor de carga de la cola actualizo el 
% weighted average

[top]
out : out
in : in
components : as@AutoScaler
link : in queueLoad@as

[as]
numberOfServers : 1
exponentialWeight : 0.6 
loadLowerBound : 0.5
loadUpperBound : 0.6
loadUpdatesToBreakIdle : 10

