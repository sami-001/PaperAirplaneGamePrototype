CC = gcc
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS)) #pattern substitute to create .o from .c
CFLAGS =
LINKER_FLAGS = -lm -lSDL2 -lSDL2_image

OUTPUT_NAME = game

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LINKER_FLAGS) -o $(OUTPUT_NAME) 

build/%.o: src/%.c
	$(CC)  -c $< $(CFLAGS) -o $@

clean:
	rm -f build/*.o $(OUTPUT_NAME)
