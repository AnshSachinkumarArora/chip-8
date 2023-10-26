objects = chip8.o main.o

chip8: $(objects)
	g++ -o chip8 $(objects)

chip8.o: chip8.h chip8.cc
	g++ -c chip8.cc 

main.o: main.cc	
	g++ -c main.cc

clean:
	rm -f chip8 *.o