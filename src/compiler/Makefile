# [mainfile] is the main file you want to begin the compiling.
# [compiler] is the c compiler used to compile the project.
#
# you can type command like "make compiler=clang" to assigned other values to the variable.
#
mainfile := cplus.c
compiler := gcc
objfiles := common.o utf.o lexer.o dynamicarr.o convert.o ident.o scope.o closectr.o \
	module.o path.o project.o parser.o expression.o ast.o delayresolve.o compiler.o

cplus: ${objfiles}
	${compiler} ${mainfile} ${objfiles} -o ${patsubst %.c, %, ${mainfile}};

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

delayresolve.o: delayresolve.h delayresolve.c
	${compiler} -c delayresolve.h delayresolve.c

clean:
	rm *.o *.gch

