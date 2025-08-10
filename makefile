char: main.cpp
	g++ -std=c++11 -Wall -O2 -I. -o main main.cpp

clean:
	rm -f main

.PHONY: clean