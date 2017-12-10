CC= g++
LDFLAGS=-lGL -lglfw -lGLEW -lm -lassimp
INCLUDES=-I.
MAGICK_FLAGS=`Magick++-config --cppflags --ldflags --libs`
CPPFLAGS=-Wall -Wextra -std=c++14 -g -fpermissive

DEPS=bin/object.o bin/texture.o bin/objloader.o bin/camera.o \
bin/transform.o bin/shaders.o bin/light.o\
bin/shadow_map.o bin/shadow.o bin/skybox.o\
bin/draw_fbo.o bin/transparency_map.o\
bin/shadow_map_omni.o

all: main

main2: src/main2.cpp $(DEPS)
	$(CC) src/main2.cpp $(DEPS) -o $@ $(LDFLAGS) $(INCLUDES) $(MAGICK_FLAGS) $(CPPFLAGS)

test: src/test.cpp $(DEPS)
	$(CC) src/test.cpp $(DEPS) -o $@ $(LDFLAGS) $(INCLUDES) $(MAGICK_FLAGS) $(CPPFLAGS)

main: src/main.cpp $(DEPS)
	$(CC) src/main.cpp $(DEPS) -o $@ $(LDFLAGS) $(INCLUDES) $(MAGICK_FLAGS) $(CPPFLAGS)

bin/%.o: src/%.cpp 
	$(CC) $? -c -o $@ $(LDFLAGS) $(INCLUDES) $(MAGICK_FLAGS) $(CPPFLAGS)
	
