CC := gcc
all: client server router serverGroup

server: server.cpp server.hpp
	g++ -w server.cpp -o server

client: client.cpp client.hpp
	g++ -pthread -w client.cpp -o client

router: router.cpp router.hpp
	g++ -pthread router.cpp -o router

serverGroup: serverGroup.cpp serverGroup.hpp
	g++ -pthread -w serverGroup.cpp -o serverGroup

.PHONY: clean

clean:
	rm -r *.o