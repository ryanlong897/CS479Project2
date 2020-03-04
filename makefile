FLAGS = -std=c++11 -Wall -larmadillo -O3
CC = g++

all: main

main: Distribution.cpp Classifier.cpp Main.cpp Image.cpp Image.hpp
	$(CC) $(FLAGS) Distribution.cpp Classifier.cpp Main.cpp -o main

clean: 
	rm -rf main
