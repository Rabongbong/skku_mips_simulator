CC=gcc
CCFLAGS=

#add Csource files here
SRCS=main.c -lm

TARGET=mips-sim

OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)

%.o:%.c
				$(CC) $(CCFLAGS) $< -c -o $@

$(TARGET): $(OBJS)
				$(CC) $(CCFLAGS) $^ -o $@
						
.PHONY=clean
.PHONY=run
run: $(TARGET)
	./$(TARGET)
clean:
				rm -f $(OBJS) $(TARGET)