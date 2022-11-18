CC = g++
FLAGS = -Wall -lpthread

build:
	$(CC) $(FLAGS) -I. -o tema1 tema1.cpp

clean:
	rm tema1

run:
	./tema1
