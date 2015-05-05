obj := cplus
compiler := gcc

cplus: common.o utf.o lexical.o dynamicarr.o convert.o ident.o scope.o \
       close_counter.o fileset.o syntax.o semantic.o errsys.o
	${compiler} ${obj}.c common.o utf.o lexical.o dynamicarr.o convert.o ident.o \
	scope.o close_counter.o fileset.o syntax.o semantic.o errsys.o -o ${obj}

common.o: common.h common.c
	${compiler} -c common.h common.c

utf.o: utf.h utf.c
	${compiler} -c utf.h utf.c

lexical.o: lexical.h lexical.c
	${compiler} -c lexical.h lexical.c

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

syntax.o: syntax.h syntax.c
	${compiler} -c syntax.h syntax.c

semantic.o: semantic.h semantic.c
	${compiler} -c semantic.h semantic.c

errsys.o: errsys.h errsys.c
	${compiler} -c errsys.h errsys.c

clean:
	rm *.o *.gch
