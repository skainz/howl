all: dynamic static

dynamic:
#	gcc -Wall -lncurses -o app app.c
#	gcc -Wall  -lncursesw  -o howl win.c mypanel.c
	gcc -Wall -g -o howl win.c mypanel.c -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 -lncursesw -ltinfo -ldl


static:
	gcc -static  -static -o howl-static win.c mypanel.c -lncursesw -ltermcap



libs:
	gcc -c -fPIC vfs/local.c -o vfs/local.o
	gcc -shared -o libvfslocal.so vfs/local.o
clean:
	rm -rf howl howl-static
