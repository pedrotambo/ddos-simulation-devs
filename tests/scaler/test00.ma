% Cuando llega el factor de carga de la cola actualizo el 
% weighted average

[top]
out : out
in : in
components : as@AutoScaler
link : in queueLoad@as

[as]
server_farm_size : 1
exponential_weight : 0.6 
load_lower_bound : 0.5
load_upper_bound : 0.6
load_updates_to_break_idle : 10

