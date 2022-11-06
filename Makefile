CC=gcc
CCFLAGS= -Wall -g -I.
HEAD = huff.h
O = huff.o heap.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

huff: $(O)
	$(CC) -o $@ $^ $(CCFLAGS)

.PHONY: clean

clean:
	rm -f *.o