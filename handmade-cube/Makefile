all: main.o
	g++ main.o -lGL -lGLEW -lglfw3 -lX11 -Wall -o build/a.out
clean:
	rm build/a.out
	rm *.o
run:
	./build/a.out

main.o:
	g++ main.cpp -c
