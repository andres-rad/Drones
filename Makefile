all: main
CXXFLAGS+=-std=c++0x
clean:
	rm -f *.o
	rm -f main

main: main.cpp tipos.o auxiliares.o campo.o drone.o sistema.o
	g++ $(CXXFLAGS) -g -Wall -o main main.cpp tipos.o auxiliares.o campo.o drone.o sistema.o

tipos.o: tipos.h tipos.cpp
	g++ $(CXXFLAGS) -g -Wall -c tipos.cpp

aux.o: auxiliares.h auxiliares.cpp
	g++ $(CXXFLAGS) -g -Wall -c auxiliares.cpp

campo.o: campo.h campo.cpp
	g++ $(CXXFLAGS) -g -Wall -c campo.cpp

drone.o: drone.h drone.cpp
	g++ $(CXXFLAGS) -g -Wall -c drone.cpp

sistema.o: sistema.h sistema.cpp
	g++ $(CXXFLAGS) -g -Wall -c sistema.cpp
