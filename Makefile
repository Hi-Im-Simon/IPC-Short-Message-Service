FLAGS = -Wall

# dependencies
COMMON_DEP = 
SERVER_DEP =
CLIENT_DEP =

all: client.out server.out

client.out: $(COMMON_DEP) $(CLIENT_DEP)
	gcc app/inf150192c.c -o client.out $(COMMON_DEP) $(CLIENT_DEP)

server.out : $(COMMON_DEP) $(SERVER_DEP)
	gcc app/inf150192s.c -o server.out $(COMMON_DEP) $(SERVER_DEP)

$(COMMON_DEP):
	gcc -c $@.c -o $@ $(FLAGS)

$(SERVER_DEP):
	gcc -c $@.c -o $@ $(FLAGS)

$(CLIENT_DEP):
	gcc -c $@.c -o $@ $(FLAGS)

clean:
	rm -f $(COMMON_DEP) $(SERVER_DEP) $(CLIENT_DEP)