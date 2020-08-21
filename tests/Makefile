CC:=gcc
CPP:=g++
CROSS_COMPILE?=

CFLAGS:=-Wall -O2 -fno-toplevel-reorder
LFLAGS:=

SRC:=$(shell find . -name "*.c")
SRC+=$(shell find . -name "*.cpp")
#SRC+=$(shell find ../src -name "*.c")
#SRC+=$(shell find ../src -name "*.cpp")
OBJ:=$(subst ../src/,./obj/,$(SRC))
OBJ:=$(subst src/,obj/,$(OBJ))
OBJ:=$(subst .cpp,.o,$(OBJ))
OBJ:=$(subst .c,.o,$(OBJ))

OUT:=out/main.elf
DIR:=$(sort $(dir $(OBJ))) $(sort $(dir $(OUT)))
INC:=-I../inc
LIB:=-L../out -l:libcompage.a -lpthread 

all:$(DIR) $(OUT)

$(DIR):
	mkdir -p $@

$(OUT):$(OBJ)
	$(CROSS_COMPILE)$(CPP) $(LFLAGS) $(OBJ) -o $(OUT) $(LIB)

obj/%.o: src/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) -c -o $@ $<

obj/%.o: src/%.cpp
	$(CROSS_COMPILE)$(CPP) $(CFLAGS) $(INC) -c -o $@ $<

obj/%.o: ../src/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) -c -o $@ $<

obj/%.o: ../src/%.cpp
	$(CROSS_COMPILE)$(CPP) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm -fr $(DIR)
