main: main.cpp
	clang++ -std=c++20 -Wall -Wextra -O2 -o chaingen main.cpp

clean:
	rm -f chaingen

.PHONY: clean