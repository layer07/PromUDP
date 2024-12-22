CC = gcc
CFLAGS = -Wall -pthread
TARGET = PromUDP

SRC = main.c metrics.c udp_listener.c http_server.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
