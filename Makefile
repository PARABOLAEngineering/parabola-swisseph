CFLAGS =  -g -Wall -fPIC
OP=$(CFLAGS)
CC=cc
CXX=g++

SWEOBJ = swedate.o swehouse.o swejpl.o swemmoon.o swemplan.o sweph.o \
         swephlib.o swecl.o swehel.o

all: swetest swetests swevents swemini parabola_wrapper parabola_benchmark

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

parabola_wrapper: parabola_wrapper.cpp libswe.a
	$(CXX) -std=c++17 -pthread -o parabola_wrapper parabola_wrapper.cpp -L. -lswe -lm -ldl

parabola_benchmark: parabola_wrapper.cpp libswe.a
	$(CXX) -std=c++17 -pthread -o parabola_benchmark parabola_wrapper.cpp -L. -lswe -lm -ldl

# --- Optional: Auto-run benchmark at install time ---

install: parabola_benchmark
	@echo "[✓] Running parabola RTM benchmark..."
	./parabola_benchmark

clean:
	rm -f *.o swetest libswe* parabola_wrapper parabola_benchmark
	cd setest && make clean

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
