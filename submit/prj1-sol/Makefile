TARGET = hamming-encode

CFLAGS = -g -Wall -std=c11
LDFLAGS = -lm

hamming-decode: $(TARGET)
	ln -s -f $< $@

$(TARGET): hamming.o main.o
	$(CC) hamming.o main.o $(LDFLAGS) -o $@

clean:
	rm -f *~ *.o main.o hamming.o 


