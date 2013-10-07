# Makefile for myls project

MKFILE    = Makefile
DEPSFILE  = ${MKFILE}.deps
NOINCLUDE = clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}
GMAKE     = gmake --no-print-directory

GCC       = gcc -g -O0 -Wall -Wextra -std=gnu99
MKDEPS    = gcc -MM
GRIND     = valgrind --leak-check=full

CSOURCE   = debugf.c dqueue.c slist.c main.c
CHEADER   = debugf.h dqueue.h slist.h
OBJECTS   = ${CSOURCE:.c=.o}
EXECBIN   = myls


all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o $@ ${OBJECTS}

%.o : %.c
	${GCC} -c $<

clean :
	- rm ${OBJECTS} ${DEPSFILE}

spotless : clean
	- rm ${EXECBIN}

test : ${EXECBIN}
	- ${GRIND} ./${EXECBIN} -l 
	- ${GRIND} ./${EXECBIN} -R 

deps : ${CSOURCE} ${CHEADER}
	@ echo "# ${DEPSFILE} created `date`" >${DEPSFILE}
	${MKDEPS} ${CSOURCE} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${GMAKE} deps
