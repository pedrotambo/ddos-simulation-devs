% Si llega un emit manda la tarea

[top]
out : out
in : in emit
components : q@ServerQueue
link : in in@q
link : emit emit@q
link : discarded@q out
link : out@q out
link : current_size@q out

[q]
size : 1
current_size_frequency : 00:00:1:000
