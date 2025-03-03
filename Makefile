all:
	gcc -o brewer coffee.h brewer.c brew.c file_open_close.c
	gcc -o recovery coffee.h recovery.c recover.c file_open_close.c

