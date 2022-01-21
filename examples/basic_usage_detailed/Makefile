CC:=gcc
CXX:=g++
CROSS_COMPILE?=

CFLAGS+=-Wall -O2 -fno-toplevel-reorder -g
LFLAGS?=

OUT:=out/main.elf
INC:=-Iinc -I../../inc
LIB:=-L../../out -l:libcompage.a -lpthread 

SRC:=$(shell find src -name "*.c" -o -name "*.cpp")
OBJ:=$(SRC:src/%=obj/%.o)
DEP:=$(OBJ:obj/%.o=dep/%.d)

DIR:=$(sort $(dir $(OBJ) $(DEP) $(OUT)))

all: $(DIR) $(OUT)

$(DIR):
	mkdir -p $@

$(OUT): ../../out/libcompage.a $(OBJ)
	$(CROSS_COMPILE)$(CXX) $(LFLAGS) $(OBJ) -o $(OUT) $(LIB)

obj/%.c.o: src/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst obj/%.o,dep/%.d,$@) -c -o $@ $<

obj/%.cpp.o: src/%.cpp
	$(CROSS_COMPILE)$(CXX) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst obj/%.o,dep/%.d,$@) -c -o $@ $<

library:
	make -C ../../

clean:
	rm -fr $(DIR) tags

distclean: clean
	make -C ../../ clean

ctags:
	ctags --language-force=c -R -o tags .

-include $(DEP)
