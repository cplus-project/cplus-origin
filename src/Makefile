obj := cplus
compiler := gcc

cplus: common.o utf.o lex.o dynamicarr.o convert.o ident.o scope.o \
       close_counter.o errors.o fileset.o parser.o
	${compiler} ${obj}.c common.o utf.o lex.o dynamicarr.o convert.o ident.o \
	scope.o close_counter.o errors.o fileset.o parser.o -o ${obj}

common.o: common.h common.c
	${compiler} -c common.h common.c

utf.o: utf.h utf.c
	${compiler} -c utf.h utf.c

lex.o: lex.h lex.c
	${compiler} -c lex.h lex.c

dynamicarr.o: dynamicarr.h dynamicarr.c
	${compiler} -c dynamicarr.h dynamicarr.c

convert.o: convert.h convert.c
	${compiler} -c convert.h convert.c

ident.o: ident.h ident.c
	${compiler} -c ident.h ident.c

scope.o: scope.h scope.c
	${compiler} -c scope.h scope.c

close_counter.o: close_counter.h close_counter.c
	${compiler} -c close_counter.h close_counter.c

errors.o: errors.h errors.c
	${compiler} -c errors.h errors.c

fileset.o: fileset.h fileset.c
	${compiler} -c fileset.h fileset.c

parser.o: parser.h parser.c
	${compiler} -c parser.h parser.c

clean:
	rm *.o *.gch
