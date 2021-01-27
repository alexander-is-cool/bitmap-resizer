CC = gcc
CFLAGS = -pipe -Wall -O2 -D_FORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS -g

resize: 
	$(CC) $(CFLAGS) resize.c -o resize.exe

copy: 
	$(CC) $(CFLAGS) copy.c -o copy.exe