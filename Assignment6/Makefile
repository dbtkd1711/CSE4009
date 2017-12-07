all : test1 test2 get_time

test1 : hybrid_lock.o test1.o
	gcc -o test1 hybrid_lock.o test1.o -lpthread
test2 : hybrid_lock.o test2.o
	gcc -o test2 hybrid_lock.o test2.o -lpthread
get_time : get_time.c
	gcc -o get_time get_time.c -lpthread

hybrid_lock.o : hybrid_lock.c
	gcc -c -o hybrid_lock.o hybrid_lock.c -lpthread

test1.o : test1.c
	gcc -c -o test1.o test1.c -lpthread

test2.o : test2.c
	gcc -c -o test2.o test2.c -lpthread

clean :
	rm *.o test1 test2 get_time
