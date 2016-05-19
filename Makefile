CFLAGS:=-O3

all: yesno bsearch

yesno: yesno.c hashdict.c
bsearch: bsearch.c

clean:
	rm -f *.o

$(info Please look for 'yesno' and 'bsearch' binaries)

