cplus: common.o lex.o dynamicarr.o
	gcc cplus.c common.o lex.o dynamicarr.o -o cplus

common.o: common.h common.c
	gcc -c common.h common.c

lex.o: lex.h lex.c
	gcc -c lex.h lex.c

dynamicarr.o: dynamicarr.h dynamicarr.c
	gcc -c dynamicarr.h dynamicarr.c

clean:
	rm *.o *.gch
