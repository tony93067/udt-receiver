all: background_server_uplink background_server_downlink server

% : %.c
	gcc -o $@ $< -lnsl -lpthread 
clean:
	rm -f server server.o
