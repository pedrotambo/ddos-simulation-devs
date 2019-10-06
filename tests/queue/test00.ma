% Le llega el trabajo a la cola

[top]
out : out
in : in 
components : q@ServerQueue
link : in in@q
link : discarded@q out
link : out@q out
link : current_size@q out

[q]
size : 10
current_size_frequency : 00:00:10:000
