include Config.mak

ifeq ($(DEBUG),yes)
  DEBUG =  -W -Wall -Wno-unused-parameter -g -DDEBUG # -pedantic
else
  DEBUG =  -w -O
endif

CFLAGS = $(STD) $(DEBUG) $(HDF5INC)

LIB = $(HDF5LIB)

OBJ =  fclib.o \
       fcmer.o \

fctst: fctst.o libfclib.a
	$(CC) $(PROFILE) -o $@ $< -L. -lfclib $(LIB)

libfclib.a: $(OBJ)
	ar rcv $@ $(OBJ)
	ranlib $@ 

clean:
	rm -f fctst
	rm -f *.o
	rm -f *.a

fctst.o: fctst.c 
	$(CC) $(CFLAGS) -c -o $@ $<

fclib.o: fclib.c fclib.h
	$(CC) $(CFLAGS) -c -o $@ $<

fcmer.o: fcmer.c fclib.h
	$(CC) $(CFLAGS) -c -o $@ $<
