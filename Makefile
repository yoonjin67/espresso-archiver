LIBS=-ltcmalloc -lz
all:
	gcc -o eszip espresso_zip.h archiver.c archive.c file_open_close.c $(LIBS)
	gcc -o esunzip espresso_zip.h extractor.c extract.c file_open_close.c $(LIBS)

