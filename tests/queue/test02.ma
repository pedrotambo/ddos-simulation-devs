% Si llega un emit manda la tarea

[top]
out : out
in : in emit
components : q@ServerQueue
link : in in@q
link : emit emit@q
link : out@q out

[q]
size : 1
currentSizeFrequency : 00:00:01:000
