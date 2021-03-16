all:
#	gcc -Wall -lncurses -o app app.c
	gcc -Wall  -g -lncursesw  -o win win.c mypanel.c
#	gcc -Wall  -g -lncursesw -ltinfo -ldl  -o panels panels.c -lpanel
#	gcc -static win.c -o win -lncursesw
#	gcc -Wall -o local_vfs local_vfs.c
#	gcc -Wall -lncurses -o demo1 demo1.c
#	gcc -Wall -lncurses -lmenu -o menu menu.c
