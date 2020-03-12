FLAGS = -std=c++11 -pthread -lpthread -Wall -Wl,--no-as-needed -larmadillo -O3 
CC = g++

all: main

Distribution.o: Distribution.cpp Distribution.hpp
	$(CC) -o Distribution.o Distribution.cpp $(FLAGS) -c

Image.o: Image.cpp Image.hpp
	$(CC) -o Image.o Image.cpp $(FLAGS) -c

Classifier.o: Distribution.o Image.o Classifier.cpp Classifier.hpp
	$(CC) -o Classifier.o Classifier.cpp $(FLAGS) -c

main: Distribution.o Classifier.o Image.o Main.cpp
	$(CC) $(FLAGS) Distribution.o Classifier.o Image.o Main.cpp -o main

clean: 
	rm -rf main *.o
