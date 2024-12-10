#CC = icc
#CLFAGS = -O3 -ip

CC = gcc
CFLAGS = -g

PROG = primos

HDRS = getopt.h calc.h
SRCS = calc.c getopt.c
FILES = primos

# significa que os ficheiros objecto t�m o mesmo nome que as sources mas com a extens�o .o
OBJS = $(SRCS:.c=.o)

$(PROG) : $(OBJS)
	$(CC) ${LDFLAGS} $(OBJS) -o $(PROG)

clean : 
	rm -f core $(PROG) $(OBJS)

cleanbin:
	rm -f $(FILES)

TAGS : $(SRCS) $(HDRS)
	etags -t $(SRCS) $(HDRS)
