#===============================================================================
# Network Sum Calculator - Build Configuration
#===============================================================================

# Compiler settings
CXX          = g++
CXXFLAGS     = -Wall -Wextra -std=c++11 -O2
LDFLAGS      =

# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    LIBS     = -lws2_32
    EXT      = .exe
    RM       = del /f /q
else
    PLATFORM = Unix
    LIBS     = 
    EXT      =
    RM       = rm -f
endif

# Targets
SERVER_BIN   = net_server$(EXT)
CLIENT_BIN   = net_client$(EXT)
SERVER_SRC   = network_server.cpp
CLIENT_SRC   = network_client.cpp

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)
	@echo "Build complete for $(PLATFORM)"

# Server
$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS) $(LDFLAGS)
	@echo "Server built: $@"

# Client
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS) $(LDFLAGS)
	@echo "Client built: $@"

# Clean
clean:
	$(RM) $(SERVER_BIN) $(CLIENT_BIN)
	@echo "Cleaned up binaries"

# Help
help:
	@echo "Available targets:"
	@echo "  all    - Build both server and client"
	@echo "  clean  - Remove binaries"
	@echo "  run-server - Start the server"
	@echo "  run-client - Run the client"

# Run targets
run-server: $(SERVER_BIN)
	@echo "Starting server..."
	@./$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	@echo "Starting client..."
	@./$(CLIENT_BIN)

.PHONY: all clean help run-server run-client
