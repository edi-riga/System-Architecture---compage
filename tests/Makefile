CC:=gcc
CXX:=g++
CROSS_COMPILE?=

CFLAGS+=-Wall -O2 -fno-toplevel-reorder -g
LFLAGS?=

OUT:=out/main.elf
INC:=-Iinc -I../inc
LIB:=-L../out -l:libcompage.a -lpthread 

SRC:=$(shell find src -name "*.c" -o -name "*.cpp")
OBJ:=$(SRC:src/%=obj/%.o)
DEP:=$(OBJ:obj/%.o=dep/%.d)

DIR:=$(sort $(dir $(OBJ) $(DEP) $(OUT)))

all:$(DIR) $(OUT)

$(DIR):
	mkdir -p $@

$(OUT): $(OBJ)
	$(CROSS_COMPILE)$(CXX) $(LFLAGS) $(OBJ) -o $(OUT) $(LIB)

obj/%.c.o: src/%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst obj/%.o,dep/%.d,$@) -c -o $@ $<

obj/%.cpp.o: src/%.cpp
	$(CROSS_COMPILE)$(CXX) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst obj/%.o,dep/%.d,$@) -c -o $@ $<

clean:
	rm -fr $(DIR) tags

ctags:
	ctags --language-force=c -R -o tags .

-include $(DEP)
