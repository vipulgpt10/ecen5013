include sources.mk

CC:=gcc
CFLAGS:= 
LDFLAGS:= 
INCLUDES:= 

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= client$(OUTPUT_FILE_EXT)


.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) $(SRCS) $(LDFLAGS)

.PHONY:run
run: build
	./client.out

.PHONY:clean
clean:
	rm -f  $(OUTPUT_FILE)
