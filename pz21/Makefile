CC = gcc
CFLAGS = -Wall -Wextra

TARGET = daemon

all: $(TARGET)

$(TARGET): daemon.c
	$(CC) $(CFLAGS) -o $(TARGET) daemon.c

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	del /f /q $(TARGET).exe daemon_keyboard.log
else
	rm -f $(TARGET) daemon_keyboard.log
endif

.PHONY: all run clean
