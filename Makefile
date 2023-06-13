CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -std=c++11 -g
LDFLAGS=-Llibs/raylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
INCLUDES=-Ilibs/raylib/include -Ilibs/imgui

SRCDIR=src
IMGUI_DIR=libs/imgui
SOURCES=$(wildcard $(SRCDIR)/*.cpp $(IMGUI_DIR)/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=wad

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
