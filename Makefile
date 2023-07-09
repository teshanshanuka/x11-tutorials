CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lX11 -lXft
INCLUDES = `pkg-config --cflags xft` # for paint app

TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS) $(INCLUDES)

clean:
	rm -f $(TARGET)
