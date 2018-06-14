
SRC = ../../src
CC = g++
CFLAGS = -std=c++11

all: generate_pcm_file

clean:
	rm *.o *.pcm generate_pcm_file

oscillators.o: $(SRC)/oscillators.cpp
	$(CC) $(CFLAGS) -c $< -o $@

envelope.o: $(SRC)/envelope.cpp
	$(CC) $(CFLAGS) -c $< -o $@

instrument.o: $(SRC)/instrument.cpp
	$(CC) $(CFLAGS) -c $< -o $@

synthesizer.o: $(SRC)/synthesizer.cpp
	$(CC) $(CFLAGS) -c $< -o $@

songreader.o: $(SRC)/songreader.cpp
	$(CC) $(CFLAGS) -c $< -o $@

songplayer.o: $(SRC)/songplayer.cpp
	$(CC) $(CFLAGS) -c $< -o $@

generate_pcm_file: main.cpp instrument.o envelope.o synthesizer.o songreader.o songplayer.o oscillators.o
	$(CC) $(CFLAGS) -I$(SRC) main.cpp instrument.o envelope.o synthesizer.o songreader.o songplayer.o oscillators.o -o generate_pcm_file

test:
	./generate_pcm_file
	@echo
	@echo "After installing sox, run this, assuming each sample is 64 bits:"
	@echo "    make play"
	@echo

play:
	play -t raw -r 44100 -e unsigned-integer -b 8 -c 1 output.pcm

