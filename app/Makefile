# FLAGS = -Wall

# dependientes powinny miec plik .h i .c
COMMON_DEPENDIENTES = 
SERVER_DEPENDIENTES =
CLIENT_DEPENDIENTES =

all: client.exe server.exe

client.exe: $(COMMON_DEPENDIENTES) $(CLIENT_DEPENDIENTES)
	gcc client.c -o client.out $(COMMON_DEPENDIENTES) $(CLIENT_DEPENDIENTES)

server.exe : $(COMMON_DEPENDIENTES) $(SERVER_DEPENDIENTES)
	gcc server.c -o server.out $(COMMON_DEPENDIENTES) $(SERVER_DEPENDIENTES)

$(COMMON_DEPENDIENTES):
	gcc -c $@.c -o $@ $(FLAGS)

$(SERVER_DEPENDIENTES):
	gcc -c $@.c -o $@ $(FLAGS)

$(CLIENT_DEPENDIENTES):
	gcc -c $@.c -o $@ $(FLAGS)

clean:
	rm -f $(COMMON_DEPENDIENTES) $(SERVER_DEPENDIENTES) $(CLIENT_DEPENDIENTES)