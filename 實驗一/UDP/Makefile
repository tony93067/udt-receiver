all: server

% : %.c
	gcc -o $@ $< -lnsl -lpthread 
clean:
	rm -f server
