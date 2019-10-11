%Nunca apago si tengo un solo servidor prendido

[top]
out : out
in : queueLoad serverResponse
components : as@AutoScaler
link : queueLoad queueLoad@as
link : serverResponse serverResponse@as 
link : serverStatus@as out

[as]
server_farm_size : 1
exponential_weight : 0.6 
load_lower_bound : 0.4
load_upper_bound : 0.5
load_updates_to_break_idle : 5
server0 : on