CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Isrc -Isrc/algorithms -Isrc/cube -Isrc/solver -Isrc/ui -Isrc/utils -Wno-deprecated-declarations
LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa -framework IOKit -framework CoreVideo

# Directories
SRC_DIRS = src src/algorithms src/cube src/solver src/ui src/utils
SRCS = $(wildcard src/*.cpp) \
       $(wildcard src/algorithms/*.cpp) \
       $(wildcard src/cube/*.cpp) \
       $(wildcard src/solver/*.cpp) \
       $(wildcard src/ui/*.cpp) \
       $(wildcard src/utils/*.cpp)

OBJS = $(SRCS:.cpp=.o)
TARGET = RubikX3D

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
