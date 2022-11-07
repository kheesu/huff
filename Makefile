CC=gcc
CCFLAGS= -Wall -O2 -I.
HEAD = huff.h
O = main.o huff.o heap.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

shc: $(O)
	$(CC) -o $@ $^ $(CCFLAGS)

.PHONY: clean

clean:
	rm -f *.o