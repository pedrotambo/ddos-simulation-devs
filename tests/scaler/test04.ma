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
server_farm_size : 3
exponential_weight : 0.6 
load_lower_bound : 0.4
load_upper_bound : 0.5
load_updates_to_break_idle : 3
server0 : on