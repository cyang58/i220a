PROJECT = key-value

SRC_FILES =	key-value.c\
	key-value.h\
	main.c\
	process-file.c\
	process-file.h\
	README

CFLAGS = -g -Wall -std=c11
LDFLAGS = -lm

$(PROJECT):	key-value.o main.o process-file.o
		$(CC) key-value.o $(LDFLAGS) process-file.o main.o -o $@

clean:
		rm -f *.o $(PROJECT) $(PROJECT).tar.gz

depend:
		$(CC) -MM $(CPPFLAGS) *.c

dist:		$(PROJECT).tar.gz

$(PROJECT).tar.gz:		$(SRC_FILES)
		tar -cf $(PROJECT).tar $(SRC_FILES)
		gzip -f $(PROJECT).tar


#automatically generated dependencies produced by make depend
key-value.o:	key-value.c key-value.h
main.o:	main.c process-file.h
process-file.o: process-file.c process-file.h key-value.h
