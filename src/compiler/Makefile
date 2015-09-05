obj := cplus
compiler := gcc

cplus: common.o utf.o lexer.o dynamicarr.o convert.o ident.o scope.o closectr.o \
	module.o path.o project.o parser.o expression.o ast.o
	${compiler} ${obj}.c common.o utf.o lexer.o dynamicarr.o convert.o ident.o \
	scope.o closectr.o module.o path.o project.o parser.o expression.o ast.o -o ${obj}

common.o: common.h common.c
	${compiler} -c common.h common.c

utf.o: utf.h utf.c
	${compiler} -c utf.h utf.c

lexer.o: lexer.h lexer.c
	${compiler} -c lexer.h lexer.c

dynamicarr.o: dynamicarr.h dynamicarr.c
	${compiler} -c dynamicarr.h dynamicarr.c

convert.o: convert.h convert.c
	${compiler} -c convert.h convert.c

ident.o: ident.h ident.c
	${compiler} -c ident.h ident.c

scope.o: scope.h scope.c
	${compiler} -c scope.h scope.c

closectr.o: closectr.h closectr.c
	${compiler} -c closectr.h closectr.c

errors.o: errors.h errors.c
	${compiler} -c errors.h errors.c

module.o: module.h module.c
	${compiler} -c module.h module.c

path.o: path.h path.c
	${compiler} -c path.h path.c

project.o: project.h project.c
	${compiler} -c project.h project.c

parser.o: parser.h parser.c
	${compiler} -c parser.h parser.c

expression.o: expression.h expression.c
	${compiler} -c expression.h expression.c

ast.o: ast.h ast.c
	${compiler} -c ast.h ast.c

clean:
	rm *.o *.gch

