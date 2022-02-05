all: solution

solution: main.o shell.o  
	g++ main.o shell.o -o unixshell

main.o: main.cpp
	g++ -c main.cpp

shell.o: shell.cpp
	g++ -c shell.cpp

clean :
	rm -rf *o hello
