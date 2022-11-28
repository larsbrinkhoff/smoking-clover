CFLAGS = -g -Wall `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0` -lm

SRCS = README COPYING ChangeLog BUGS Makefile *.[ch]

CLOVER_OBJS = clover.o common.o direct.o true.o rgb.o
clover: $(CLOVER_OBJS)
	$(CC) $(CFLAGS) -o clover $(CLOVER_OBJS) $(LIBS)

$(CLOVER_OBJS): clover.h Makefile

tar:
	tar -cf - $(SRCS) | gzip > clover.tar.gz

clean:
	rm -f *.o clover
