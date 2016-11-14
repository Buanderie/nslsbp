CC=gcc
CFLAGS=-I.
LIBS=-lwiringPi
DEPS = cc_low_level.h rs_work.h spidev.h wiringPi.h error_def.h 
OBJ = cc_low_level.o cc_control.o rs_work.o spidev.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cc_daemon: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ 
