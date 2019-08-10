sudo-kit: sudo-kit.c
	$(CC) -Wall -o sudo-kit.out sudo-kit.c

static: sudo-kit.c
	$(CC) -Wall -Os -o sudo-static.out sudo-kit.c -static

clean:
	rm sudo*.out

debug:	sudo-kit.c
	# Used if you want debugging features on
	$(CC) -Wall -Wextra -Og -g -DDEBUGGING -o sudo-debug.out sudo-kit.c

