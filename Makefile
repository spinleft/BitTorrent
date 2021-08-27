INCLUDE = -I ./includes

objects = main.o

vpath %.hpp includes includes/torrent includes/util
vpath %.cpp src src/util test

test: $(objects)
	g++ -o main $(objects)
	./main
main.o: main.cpp torrent.hpp
	g++ $(INCLUDE) -c test/main.cpp
torrent.hpp: bencoding.hpp

.PHONY: test
clean:
	rm $(objects) main