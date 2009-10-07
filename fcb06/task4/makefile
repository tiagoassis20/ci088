SMPL=~/aulas/elias/SD/lib/smpl
RAND=~/aulas/elias/SD/lib/rand
FILE=adaptive-DSD


all: ${FILE}

${FILE}: ${FILE}.o smpl.o rand.o
	$(LINK.c) -o $@ -Bstatic ${FILE}.o smpl.o rand.o -lm

smpl.o: ${SMPL}.c ${SMPL}.h
	$(COMPILE.c)  -g ${SMPL}.c

${FILE}.o: ${FILE}.c ${SMPL}.h
	$(COMPILE.c) -g  ${FILE}.c

rand.o: ${RAND}.c
	$(COMPILE.c) -g ${RAND}.c

clean:
	$(RM) *.o ${FILE} relat saida

