% La cola emite periodicamente su factor de carga

[top]
out : out
in : in emit
components : q@ServerQueue
link : in in@q
link : emit emit@q
link : out@q out

[q]
size : 10
currentSizeFrequency : 00:00:05:000
