include sources.mk

CC:=gcc
CFLAGS:= 
LDFLAGS:= -lpthread -lrt -lm -lmraa
INCLUDES:= -I ../include/common -I ../include/bbg

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= project1$(OUTPUT_FILE_EXT)


.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) $(SRCS) $(LDFLAGS)

.PHONY:run
run: build
	sudo ./project1.out

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE)
