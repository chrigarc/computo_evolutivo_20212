planificador: planificador.c
	gcc -O2 -o planificador planificador.c

test: planificador
	time ./planificador MachinesFile Taskfile 1 2

clean:
	rm -f planificador output
