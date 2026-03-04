CC = gcc
CFLAGS = -Wall -Wextra

TARGET = shadow

all: $(TARGET)

$(TARGET): shadow_logger.c
	$(CC) $(CFLAGS) -o $(TARGET) shadow_logger.c

run: $(TARGET)
	@echo "Shadow rises..."
	@./$(TARGET)
	@echo "Shadow walks in the background..."

clean:
ifeq ($(OS),Windows_NT)
	del /f /q $(TARGET).exe shadow_trail.log
else
	rm -f $(TARGET) shadow_trail.log
	@echo "Shadow has been vanquished (files cleaned)"
endif

watch:
	@echo "Watching shadow's trail..."
	@tail -f shadow_trail.log

.PHONY: all run clean watch
