LIBS=-lncurses -pthread
FLAGS=-g -o
main:
	g++ $(FLAGS) main.out main.cpp Road.cpp Car.cpp $(LIBS)

clean:
	rm -f *.o