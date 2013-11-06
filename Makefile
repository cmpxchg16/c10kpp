CXX = g++
CXXFLAGS = -g -std=c++11 -Wall
CXXFLAGS_THREADS = -g -std=c++11 -Wall -DUSE_AFFINITY -DPTHREAD_IPC
LDFLAGS = -lboost_system -L. -fPIC -shared -pthread -lboost_thread
LDFLAGS_THREADS = -fPIC -pthread
SRC = asio-http-server/src/*.cpp
OBJ = $(notdir $(SRC:.cpp=.o))
TARGET = libasio-http-server.so
BIN = asio-http-server/bin/asio-http-server
BIN_THREADS = threads-http-server/bin/threads-http-server

.PHONY: $(TARGET) all clean

all: $(TARGET) $(BIN) $(BIN_THREADS)

$(BIN):
	$(CXX) -c asio-http-server/src/main/main.cpp $(CXXFLAGS)
	$(CXX) -o $(BIN) -L. main.o -lboost_system -lboost_thread -lboost_program_options -pthread -lasio-http-server

$(TARGET): $(OBJ)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

$(OBJ): $(SRC) 
	$(CXX) -c $^ $(CXXFLAGS) -fPIC

$(BIN_THREADS):
	$(CXX) -o $(BIN_THREADS) threads-http-server/src/server.cpp $(CXXFLAGS) $(LDFLAGS_THREADS)

clean:
	rm -fr *.o libasio-http-server.so asio-http-server/bin/* threads-http-server/bin/*
