#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
 
int main(int argc, char **argv) {

  char *machinesFile = argv[1];
  char *tasksFile = argv[2];

  int totalRecursos = atoi(argv[3]);
  int totalTareas = atoi(argv[4]);

  FILE *fTasks, *fMachines; //Variables para manejo de archivos

  fTasks = fopen(tasksFile, "r"); //Abre para lectura
  fMachines = fopen(machinesFile, "r"); //Abre para lectura


  size_t bufsize = 2000;
  char tareaActual[bufsize];
  char *buffer = tareaActual; 

  // Init lista de recursos disponibles
  char *machines[totalRecursos];
  int lista_recursos[totalRecursos];
  // printf("Leyendo lista de recuros disponibles...\n");

  ssize_t read;
  size_t len = 0;

  for(int i =0; i < totalRecursos; i++) {
    machines[i] = malloc(sizeof(char)*bufsize);
    read = getline(&machines[i], &len, fMachines);
    machines[i][strlen(machines[i]) - 1] = '\0';
    lista_recursos[i] = 0;
  }
  fclose(fMachines);

  char buffArg[2000];

  char *args[] = {"/bin/sh", "-c", buffArg, NULL};

  int status;

  pid_t pidActual, recurso_disp;
 
  int numProcesos = 0;
  int enEjecucion = 0;

  while ( numProcesos < totalTareas) {
    getline(&buffer, &bufsize, fTasks);
    buffer[strlen(buffer) - 1] = '\0';

    // Buscar un recurso disponible
    recurso_disp = -1;

    for(int i=0; i < totalRecursos; i++ ) {
      if (lista_recursos[i] == 0) {
        recurso_disp = i;
        break;
      }
    }


    if (recurso_disp < 0) {
      // Espera a que se libere un recurso:
      pid_t idTermino = wait(&status);
      //printf("Termino de ejecutar el proceso: %d \n", pidActual);
      enEjecucion--;

      // Una vez que un proceso termino, buscamos el recurso libre
      // Y lo liberamos:
      for(int i=0; i < totalRecursos; i++) {
        if (lista_recursos[i] == idTermino) {
         recurso_disp = i;
         lista_recursos[i] = 0;
        }
      }
    }

    usleep(10000); // Espera 0.01 segundos
    pidActual = fork();
  
    if ( pidActual == 0 ) { /* hijo */
      sprintf(args[2], "%s", buffer);  //esta linea es para no depender del ssh y ejecutar directamente en local

      // Ejecuta el programa usando recurso_dis
      execv(args[0], args);

    } else { /*  padre */
      // registra el proceso creado:
      numProcesos++;
      enEjecucion++;
      // Registra recurso_dis ocupado por el proceso creado:
      lista_recursos[recurso_disp] = pidActual;
    }    
  }
  for(int i=0; i < enEjecucion; i++) {
    pidActual = wait(&status);
  }
  fclose(fTasks);

  for(int i =0; i < totalRecursos; i++) {
    free(machines[i]);
  }

  return 0;
}
