CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lX11 -lXft

TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	rm -f $(TARGET)
