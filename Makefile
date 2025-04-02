LIBS= `pkg-config --libs --cflags libtcmalloc` -lz
all:
	gcc -o esunzip extractor.c extract.c file_open_close.c $(LIBS)
	gcc -o eszip archiver.c archive.c file_open_close.c $(LIBS)
	gcc -o generate_xor_keys generate_xor_keys.c

