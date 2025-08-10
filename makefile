char: chartranslate.cpp
	g++ -std=c++11 -Wall -O2 -I. -o char chartranslate.cpp

clean:
	rm -f char

.PHONY: clean