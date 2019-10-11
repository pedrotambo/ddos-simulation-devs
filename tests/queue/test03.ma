% Si piden paquete a cola vacia agendo el pedido
% envio ni bien llega un job

[top]
out : out
in : in emit
components : q@ServerQueue
link : in in@q
link : emit emit@q
link : out@q out

[q]
size : 1
current_size_frequency : 00:00:01:000
