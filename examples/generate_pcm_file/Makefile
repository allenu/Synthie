
SRC = ../../src
CC = g++
CFLAGS = -std=c++11

all: generate_pcm_file

clean:
	rm *.o

synthie.o: $(SRC)/synthie.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

oscillators.o: $(SRC)/oscillators.cpp
	$(CC) $(CFLAGS) -c $< -o $@

generate_pcm_file: main.cpp synthie.o oscillators.o
	$(CC) $(CFLAGS) -I$(SRC) main.cpp synthie.o oscillators.o -o generate_pcm_file

test:
	./generate_pcm_file
	@echo
	@echo "After installing sox, run this, assuming each sample is 64 bits:"
	@echo "    make play"
	@echo

play:
	play -t raw -r 44100 -e unsigned-integer -b 8 -c 1 output.pcm

