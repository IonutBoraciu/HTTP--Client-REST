CC=gcc
CFLAGS=-I.

# Parson library files
PARSON_SRC=parson.c
PARSON_HEADER=parson.h

# Target executable
client: client.c requests.c helpers.c buffer.c $(PARSON_SRC)
	$(CC) -o client client.c requests.c helpers.c buffer.c $(PARSON_SRC) $(CFLAGS) -Wall

# Run the client
run: client
	./client

# Clean up object files and executable
clean:
	rm -f *.o client

