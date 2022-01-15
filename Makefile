CC:=gcc
CPP:=g++
CROSS_COMPILE?=

CFLAGS?=
CFLAGS:=-Wall -fPIC $(CFLAGS)
LFLAGS?=
LFLAGS:=$(LFLAGS)
AFLAGS:=rcs
DEFINES?=

SRC:=$(shell find ./src -name "*.c")
SRC+=$(shell find ./src -name "*.cpp")
SRC+=$(shell find ./lib -name "*.c")
SRC+=$(shell find ./lib -name "*.cpp")
OBJ:=$(subst src/,obj/,$(SRC))
OBJ:=$(subst lib/,obj/,$(OBJ))
OBJ:=$(subst .cpp,.o,$(OBJ))
OBJ:=$(subst .c,.o,$(OBJ))

OUT:=out/libcompage.a out/libcompage.so
DIR:=$(sort $(dir $(OBJ))) $(sort $(dir $(OUT)))
INC:=-Iinc -Ilib
LIB:=-lpthread

.POSIX:

all: $(OUT) done
#all: $(OUT) compile_tests done

done:
	@echo
	@echo "Librares: $(OUT)"
	@echo "Tests:    tests"

compile_tests:
	@make -C tests

$(DIR):
	mkdir -p $@

out/libcompage.a: $(DIR) $(OBJ)
	$(CROSS_COMPILE)$(AR) $(AFLAGS) $@ $(OBJ)

out/libcompage.so: $(DIR) $(OBJ)
	$(CROSS_COMPILE)$(CPP) -o $@ $(OBJ) -shared

obj/%.o: src/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) $(DEFINES) -c -o $@ $<

obj/%.o: src/%.cpp
	$(CROSS_COMPILE)$(CPP) $(CFLAGS) $(INC) $(DEFINES) -c -o $@ $<

obj/ini/%.o: lib/ini/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) $(DEFINES) -c -o $@ $<

clean:
	rm -fr $(DIR)
	@make clean -C tests
