cplus: common.o lex.o dynamicarr.o convert.o
	gcc cplus.c common.o lex.o dynamicarr.o convert.o -o cplus

common.o: common.h common.c
	gcc -c common.h common.c

lex.o: lex.h lex.c
	gcc -c lex.h lex.c

dynamicarr.o: dynamicarr.h dynamicarr.c
	gcc -c dynamicarr.h dynamicarr.c

convert.o: convert.h convert.c
	gcc -c convert.h convert.c
    
clean:
	rm *.o *.gch
