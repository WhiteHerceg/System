CXX = g++
CXXFLAGS = -Wall -Wextra

ifeq ($(OS),Windows_NT)
    LIBS = -lws2_32
    EXT = .exe
else
    LIBS = 
    EXT =
endif

SERVER = server$(EXT)
CLIENT = client$(EXT)

all: $(SERVER) $(CLIENT)

$(SERVER): server.cpp
	$(CXX) $(CXXFLAGS) -o $(SERVER) server.cpp $(LIBS)

$(CLIENT): client.cpp
	$(CXX) $(CXXFLAGS) -o $(CLIENT) client.cpp $(LIBS)

clean:
ifeq ($(OS),Windows_NT)
	del /f /q $(SERVER) $(CLIENT)
else
	rm -f $(SERVER) $(CLIENT)
endif

.PHONY: all clean
