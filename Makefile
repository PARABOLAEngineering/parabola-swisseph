CFLAGS =  -g -Wall -fPIC
OP=$(CFLAGS)
CC=cc
CXX=g++

SWEOBJ = swedate.o swehouse.o swejpl.o swemmoon.o swemplan.o sweph.o \
         swephlib.o swecl.o swehel.o

all: swetest swetests swevents swemini libparabola.a

# --- Swiss Ephemeris tools ---

swetest: swetest.o libswe.a
	$(CC) $(OP) -o swetest swetest.o -L. -lswe -lm -ldl

swetests: swetest.o $(SWEOBJ)
	$(CC) $(OP) -static -L/usr/lib/x86_64-redhat-linux6E/lib64/ -o swetests swetest.o $(SWEOBJ) -lm -ldl

swevents: swevents.o $(SWEOBJ)
	$(CC) $(OP) -o swevents swevents.o $(SWEOBJ) -lm -ldl

swemini: swemini.o libswe.a
	$(CC) $(OP) -o swemini swemini.o -L. -lswe -lm -ldl

libswe.a: $(SWEOBJ)
	ar rcs libswe.a $(SWEOBJ)

libswe.so: $(SWEOBJ)
	$(CC) -shared -o libswe.so $(SWEOBJ)

# --- Parabola wrappers ---

# Parabola high-speed wrapper build
parabola_wrapper.o: parabola_wrapper.cpp parabola_wrapper.h
	$(CXX) -std=c++17 -pthread -c parabola_wrapper.cpp -o parabola_wrapper.o

libparabola.a: parabola_wrapper.o
	ar rcs libparabola.a parabola_wrapper.o


# --- Dependency headers ---

swecl.o: swejpl.h sweodef.h swephexp.h swedll.h sweph.h swephlib.h
sweclips.o: sweodef.h swephexp.h swedll.h
swedate.o: swephexp.h sweodef.h swedll.h
swehel.o: swephexp.h sweodef.h swedll.h
swehouse.o: swephexp.h sweodef.h swedll.h swephlib.h swehouse.h
swejpl.o: swephexp.h sweodef.h swedll.h sweph.h swejpl.h
swemini.o: swephexp.h sweodef.h swedll.h
swemmoon.o: swephexp.h sweodef.h swedll.h sweph.h swephlib.h
swemplan.o: swephexp.h sweodef.h swedll.h sweph.h swephlib.h swemptab.h
sweph.o: swejpl.h sweodef.h swephexp.h swedll.h sweph.h swephlib.h
swephlib.o: swephexp.h sweodef.h swedll.h sweph.h swephlib.h
swetest.o: swephexp.h sweodef.h swedll.h
swevents.o: swephexp.h sweodef.h swedll.h
