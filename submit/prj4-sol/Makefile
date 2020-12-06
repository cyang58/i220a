TARGET =	y86-sim

OBJS =	\
	main.o \
	ysim.o

CC = gcc
CFLAGS = -std=c11 -g -Wall
CPPFLAGS = -I $$HOME/cs220/include
LDFLAGS = -L $$HOME/cs220/lib -l cs220 -l y86
INCLUDE =	/home/cyang58/cs220/include


$(TARGET):	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS)  -o $@

ysim.o:		ysim.c ysim.h

main.o:		main.c ysim.h $(INCLUDE)/y86.h $(INCLUDE)/yas.h $(INCLUDE)/errors.h

.PHONY:
	
clean:		
		rm -f *~ *.o $(TARGET) 