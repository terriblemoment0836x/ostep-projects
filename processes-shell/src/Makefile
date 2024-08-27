CFLAGS = -g
wish: main.o parser.o utils.o execute.o
	gcc $(CFLAGS) main.o parser.o utils.o execute.o -o wish

main.o: main.c
	gcc $(CFLAGS) -c -o main.o main.c

parser.o: parser.c
	gcc $(CFLAGS) -c -o parser.o parser.c

utils.o: utils.c
	gcc $(CFLAGS) -c -o utils.o utils.c

execute.o: execute.c
	gcc $(CFLAGS) -c -o execute.o execute.c

clean:
	rm parser.o main.o utils.o execute.o
.PHONE: clean
