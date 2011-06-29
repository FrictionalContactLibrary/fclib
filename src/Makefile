include Config.mak

ifeq ($(DEBUG),yes)
  DEBUG =  -W -Wall -Wno-unused-parameter -g -DDEBUG # -pedantic
else
  DEBUG =  -w -O
endif

CFLAGS = $(STD) $(DEBUG) $(HDF5INC) $(MPIINC)

LIB = $(HDF5LIB) $(MPILIB)

OBJ =  fclib.o \
       fcmer.o \

./tests/fctst: ./tests/fctst.o libfclib.a
	$(CC) $(PROFILE) -o $@ $< -L. -lfclib $(LIB)

libfclib.a: $(OBJ)
	ar rcv $@ $(OBJ)
	ranlib $@ 

clean:
	rm -f ./tests/fctst
	rm -f ./tests/*.o
	rm -f *.o
	rm -f *.a

./tests/fctst.o: ./tests/fctst.c 
	$(CC) $(CFLAGS) -I. -c -o $@ $<

fclib.o: fclib.c fclib.h
	$(CC) $(CFLAGS) -c -o $@ $<

fcmer.o: fcmer.c fclib.h
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	./fctst
