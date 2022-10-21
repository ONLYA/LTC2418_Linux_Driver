CROSS_COMPILE :=

all:
	${CROSS_COMPILE}gcc -I. -o main *.c

clean:
	rm main