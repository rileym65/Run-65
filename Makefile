PROJECT=run65
CC=gcc
DEFS=
INCDIR=-I../
INCS=
LIBDIR=
LIBS=
OBJS=\
	cpu6502.o \
	debugger.o \
	disassem.o \
	loadfile.o \
	main.o

$(PROJECT): $(OBJS)
	$(CC) $(DEFS) $(OBJS) $(LIBDIR) $(LIBS) -o $(PROJECT)

.c.o:
	$(CC) $(DEFS) $(INCDIR) $(INCS) -c $<

clean:
	-rm *.o
	-rm $(PROJECT)

cpu6502.o:      header.h cpu6502.h cpu6502.c
debugger.o:     header.h debugger.c
disassem.o:     header.h disassem.c
loadfile.o:     header.h loadfile.c
main.o:         header.h cpu6502.h main.c
