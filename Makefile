
OBJS = src/main.c res/

CC = gcc

CFLAGS = 

LINKER_FLAGS = -lm -lSDL2 -lSDL2_image

OBJS_NAME = output

all : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LINKER_FLAGS) -o $(OBJS_NAME) 
