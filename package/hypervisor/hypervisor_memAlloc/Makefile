LIBS=-lxenstore -lxenlight -lxenctrl -lxenstat
SRC=memAlloc.c queue.c

all: __memAlloc__
__memAlloc__: $(OBJ)
	gcc -Wall -o memAlloc $(LIBS) $(SRC)
clean:
	rm -rf memAlloc
