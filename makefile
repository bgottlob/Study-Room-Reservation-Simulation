simulator.out : simulator.o user.o queue.o reservation.o
	gcc -lsqlite3 simulator.o user.o queue.o reservation.o -o simulator.out

simulator.o : simulator.c
	gcc -c simulator.c -o simulator.o

user.o : user.c
	gcc -c user.c -o user.o

queue.o : queue.c
	gcc -c queue.c -o queue.o

reservation.o : reservation.c
	gcc -c reservation.c -o reservation.o
