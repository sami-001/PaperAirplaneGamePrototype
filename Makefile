
CC = gcc
OBJS = src/main.c
RES = res/a
CFLAGS = 
LINKER_FLAGS = -lm -lSDL2 -lSDL2_image

OBJS_NAME = main

 all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LINKER_FLAGS) -o $(OBJS_NAME) 
	








#all : $(OBJS)
#	$(CC) $(OBJS) $(CFLAGS) $(LINKER_FLAGS) -o $(OBJS_NAME) 
