CC = gcc
LIBS =
CFLAGS = -xc -pedantic -std=c99 -Wall
OUTFILE = pt
OBJS = main.o prayertimes.o atan.o
SRCS = main.c prayertimes.c atan.c

all: $(OUTFILE) 

$(OUTFILE): $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $(OUTFILE)
	
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
	
clean:
	rm -rf *o $(OUTFILE)

