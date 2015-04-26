obj := cplus
compiler := gcc

cplus: common.o utf.o lex.o dynamicarr.o convert.o id.o scope.o \
       close_counter.o func.o type.o errors.o ast.o parser.o
	${compiler} ${obj}.c common.o utf.o lex.o dynamicarr.o convert.o id.o \
    scope.o close_counter.o func.o type.o errors.o ast.o parser.o -o ${obj}

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

id.o: id.h id.c
	${compiler} -c id.h id.c

scope.o: scope.h scope.c
	${compiler} -c scope.h scope.c

close_counter.o: close_counter.h close_counter.c
	${compiler} -c close_counter.h close_counter.c

func.o: func.h func.c
	${compiler} -c func.h func.c

type.o: type.h type.c
	${compiler} -c type.h type.c

errors.o: errors.h errors.c
	${compiler} -c errors.h errors.c

ast.o: ast.h ast.c
	${compiler} -c ast.h ast.c

parser.o: parser.h parser.c
	${compiler} -c parser.h parser.c

clean:
	rm *.o *.gch
