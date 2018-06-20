
SRC = ../../src
CC = g++
CFLAGS = -std=c++11

PLAY_AVAILABLE := $(shell command -v play 2> /dev/null)

all: song2pcm

clean:
	rm *.o *.pcm song2pcm *.song

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

song_from_file.o: song_from_file.cpp
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

song2pcm: main.cpp instrument.o envelope.o synthesizer.o songreader.o songplayer.o oscillators.o song_from_file.o
	$(CC) $(CFLAGS) -I$(SRC) main.cpp instrument.o envelope.o synthesizer.o songreader.o songplayer.o oscillators.o song_from_file.o -o song2pcm

test:
	./makesong.rb example_song.txt example.song
	./song2pcm example.song example.pcm
ifndef PLAY_AVAILABLE
		$(error "Can't continue playback test. Need to install sox to play example.pcm http://sox.sourceforge.net/")
endif
	play -t raw -r 44100 -e unsigned-integer -b 8 -c 1 example.pcm

