# Makefile for myls project

GCC       = gcc -g -O0 -Wall -Wextra -std=gnu99

CSOURCE   = debugf.c slist.c dstack.c main.c
CHEADER   = debugf.h slist.h dstack.h
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

deps : ${CSOURCE} ${CHEADER}
	@ echo "# ${DEPSFILE} created `date`" >${DEPSFILE}
	${MKDEPS} ${CSOURCE} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${GMAKE} deps
