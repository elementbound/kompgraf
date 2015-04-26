CXX_FLAGS = -std=c++11

build: 
	g++ ${CXX_FLAGS} -c model.cpp -o bin/model.o
	g++ ${CXX_FLAGS} -c main.cpp -o bin/main.o

	g++ bin/*.o -o subdiv