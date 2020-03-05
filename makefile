FLAGS = -std=c++11 -Wall -larmadillo -O3
CC = g++

all: main

main: Distribution.cpp Classifier.cpp Main.cpp Image.cpp
	$(CC) $(FLAGS) Distribution.cpp Classifier.cpp Image.cpp Main.cpp -o main

clean: 
	rm -rf main
