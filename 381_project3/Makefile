# usage: if this file is named "Makefile", then the commands are:
#	"make" will build the specified executable (PROG)
#	"make clean" will delete all of the .o and .exe files
#
# if this file is named something else, then use the -f option for make:
#	"make -f makefilename clean"

# set the variables below depending on compiler and options

CC = g++
LD = g++

# add -g to the CFLAGS for debugging (or -ggdb if you use gdb). 
# -pedantic-errors attempts to enforce Standard
# -Wall asks for certain warnings of possible errors
# -c is required to specify compile-only (no linking)

CFLAGS = -std=c++14 -g -pedantic-errors -Wall -c

#no load flags defined, but -l would be used to include a special library
LFLAGS = 

OBJS =  p3_main.o Room.o Person.o Meeting.o Utility.o
PROG = proj3exe

default: $(PROG)

ptest: Utility.o Person_test.o Person.o
	$(LD) $(LFLAGS) Utility.o Person.o Person_test.o -o ptest

mtest: Utility.o Meeting_test.o Meeting.o Person.o
	$(LD) $(LFLAGS) Utility.o Person.o Meeting.o Meeting_test.o -o mtest

rtest: Utility.o Room_test.o Person.o Room.o Meeting.o
	$(LD) $(LFLAGS) Utility.o Person.o Room.o Room_test.o Meeting.o -o rtest

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

p3_main.o: p3_main.cpp Room.h Meeting.h Person.h Utility.h 
	$(CC) $(CFLAGS) p3_main.cpp

Room.o: Room.cpp Room.h Meeting.h Utility.h
	$(CC) $(CFLAGS) Room.cpp

Meeting.o: Meeting.cpp Meeting.h Person.h Utility.h 
	$(CC) $(CFLAGS) Meeting.cpp

Person.o: Person.cpp Person.h Utility.h 
	$(CC) $(CFLAGS) Person.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(CFLAGS) Utility.cpp

Person_test.o: Person_test.cpp Utility.h Person.h 
	$(CC) $(CFLAGS) Person_test.cpp

Meeting_test.o: Meeting_test.cpp Utility.h Meeting.h 
	$(CC) $(CFLAGS) Meeting_test.cpp

Room_test.o: Room_test.cpp Utility.h Room.h Meeting.h 
	$(CC) $(CFLAGS) Room_test.cpp

clean:
	rm -f *.o ptest mtest rtest $(PROG)
real_clean:
	rm -rf *.o $(PROG)
