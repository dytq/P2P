CC := gcc
CFLAGS := -g
TARGET := p2p
SRCS := main.c listener.c talker.c parser.c
OBJS := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c listener.h talker.h parser.h
	$(CC) $(CFLAGS) -c $<

listener.o: listener.c listener.h
	$(CC) $(CFLAGS) -c $<

talker.o: talker.c talker.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c $<

run_server_central: $(TARGET)
	./$(TARGET) localhost 6666

run_client: $(TARGET)
	./$(TARGET) localhost 6666 192.168.0.25 6666

clean:
	rm -f $(OBJS) $(TARGET)

