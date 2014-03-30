simulator.out : simulator.o user.o queue.o reservation.o
	gcc -g -lsqlite3 -lpthread simulator.o user.o queue.o reservation.o -o simulator.out

simulator.o : simulator.c
	gcc -g -c simulator.c -o simulator.o

user.o : user.c
	gcc -g -c user.c -o user.o

queue.o : queue.c
	gcc -g -c queue.c -o queue.o

reservation.o : reservation.c
	gcc -g -c reservation.c -o reservation.o
