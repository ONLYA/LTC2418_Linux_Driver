CROSS_COMPILE :=

all:
	${CROSS_COMPILE}gcc -I. -o main *.c

debug:
	${CROSS_COMPILE}gcc -g3 -O0 -I. -o main *.c

verbose:
	${CROSS_COMPILE}gcc -g3 -O0 -D_SPI_VERBOSE_ -I. -o main *.c

clean:
	if test -f "main"; then rm main; fi