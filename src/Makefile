include Config.mak

ifeq ($(DEBUG),yes)
  DEBUG =  -W -Wall -Wno-unused-parameter -g -DDEBUG # -pedantic
else
  DEBUG =  -w -O
endif

ifneq ($(SUITESPARSE_DIR),)
  SUITESPARSE_INC = -I$(SUITESPARSE_DIR)
  LIB += $(SUITESPARSE_LIB) -lm
endif

CFLAGS = $(STD) $(DEBUG) $(SUITESPARSE_INC) $(DEFS) $(HDF5INC) $(MPIINC)

LIB += $(HDF5LIB) $(MPILIB)

OBJ =  fclib.o $(SUITESPARSE_OBJ)

all: ./tests/fctest1 ./tests/fctst_merit

./tests/fctest1: ./tests/fctst.o libfclib.a
	$(CC) $(PROFILE) -o $@ $< -L. -lfclib $(LIB)

./tests/fctst_merit: ./tests/fctst_merit.o libfclib.a
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

./tests/fctst_merit.o: ./tests/fctst_merit.c
	$(CC) $(CFLAGS) -I. -c -o $@ $<

$(SUITESPARSE_OBJ): $(SUITESPARSE_DIR)/csparse.c
	$(CC) $(CFLAGS) -I. -c -o $@ $<

fclib.o: fclib.c fclib.h
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	./fctst

.PHONY: clean all test
