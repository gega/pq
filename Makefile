#

all:	pt t1 t2

pt:	pt.c pq.h
	$(CC) -o pt pt.c

t1:	t1.c pq.h
	$(CC) -o t1 t1.c

t2:	t2.c pq.h
	$(CC) -o t2 t2.c

test:	t1 t2
	./t1
	./t2

clean:
	rm -f t1 t2 *.o pt
