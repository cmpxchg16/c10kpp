CXX = g++
CXXFLAGS = -g -std=c++11 -Wall
LDFLAGS = -lboost_system -L. -fPIC -shared -pthread -lboost_thread
SRC = src/*.cpp
HEADERS = src/*.hpp
OBJ = $(notdir $(SRC:.cpp=.o))
TARGET = libasio-server.so
BIN = asio-server

.PHONY: $(TARGET) all clean

all: $(TARGET) $(BIN)

$(BIN):
	$(CXX) -c src/main/main.cpp $(CXXFLAGS)
	$(CXX) -o $(BIN) -L. main.o -lboost_system -lboost_thread -lboost_program_options -pthread -lasio-server

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

$(OBJ): $(SRC) 
	$(CXX) -c $^ $(CXXFLAGS) -fPIC

clean:
	rm -fr *.o libasio-server.so asio-server
