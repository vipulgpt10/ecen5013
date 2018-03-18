include sources.mk

CC:=gcc
CFLAGS:= -g3
LDFLAGS:= -lpthread -lrt -lm -lmraa
INCLUDES:= -I ../include/common -I ../include/bbg ../include/cmocka

OUTPUT_FILE_EXT:=.out
OUTPUT_FILE:= project1$(OUTPUT_FILE_EXT)

######### Compler Variables for Unit Testing
TEST_SRC= light_task.c \
temp_task.c \
unittest_light.c \
unittest_temp.c \
unittest_main.c

TEST_MAIN_OUT= test_main.out

.PHONY:build
build:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT_FILE) $(SRCS) $(LDFLAGS)

.PHONY:unittest
unittest:
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_MAIN_OUT) $(TEST_SRC) $(LDFLAGS)

.PHONY:run
run: build
	sudo ./project1.out

.PHONY:clean
clean:
	rm -f *.o $(OUTPUT_FILE) $(TEST_MAIN_OUT)
