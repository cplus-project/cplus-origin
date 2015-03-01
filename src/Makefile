obj := cplus
compiler := gcc

cplus: common.o utf.o lex.o dynamicarr.o convert.o idtable.o scope.o close_counter.o ast.o syntax.o
	${compiler} ${obj}.c common.o utf.o lex.o dynamicarr.o convert.o idtable.o scope.o close_counter.o ast.o syntax.o -o ${obj}

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
	
idtable.o: idtable.h idtable.c
	${compiler} -c idtable.h idtable.c

scope.o: scope.h scope.c
	${compiler} -c scope.h scope.c
	
close_counter.o: close_counter.h close_counter.c
	${compiler} -c close_counter.h close_counter.c
	
ast.o: ast.h ast.c
	${compiler} -c ast.h ast.c
	
syntax.o: syntax.h syntax.c
	${compiler} -c syntax.h syntax.c

clean:
	rm *.o *.gch
