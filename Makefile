CC=gcc
CFLAGS=-std=c11 -Wall -Werror
LDLIBS=-lm
BFILES= zadanie proc_serv2 proc_d proc_t proc_p1 proc_p2
OEFILS= proc_pr.err proc_pr.out proc_serv1.err proc_serv1.out proc_t.err proc_t.out proc_s.err proc_s.out serv2.txt

all:$(BFILES)

proc_p1: proc_p1.c
	$(CC) $(CLAGS) proc_p1.c -o proc_p1


proc_p2: proc_p2.c
	$(CC) $(CLAGS) proc_p2.c -o proc_p2


proc_d: proc_d.c
	$(CC) $(CLAGS) proc_d.c -o proc_d


proc_t: proc_t.c
	$(CC) $(CLAGS) proc_t.c -o proc_t


zadanie: zadanie.c
	$(CC) $(CLAGS) zadanie.c -o zadanie


proc_serv2: proc_serv2.c
	$(CC) $(CLAGS) proc_serv2.c -o proc_serv2

clean:
	$(CC) $(CLAGS) -f $(BFILES) $(OEFILS)
