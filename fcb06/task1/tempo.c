#include <stdio.h>
#include <stdlib.h>
#include "../lib/smpl.h"
/* Neste programa, cada nodo conta seu tempo */

/* Eventos */

#define test 0
#define fault 1
#define repair 2

/* Descritor do nodo */

typedef struct{
  int id; /* Itentificador do nodo */
}Tnodo;

Tnodo *nodo;

int main(int argc, char *argv[]){
  static int N, /* Numero de nodos */
    token,
    event,
    r, i;

  static char fa_name[5]; /* Facility Name */

  if(argc != 2){
    printf("USO: %s <Num_nodos>\n", argv[0]);
    exit(1);
  }

  N = atoi(argv[1]);
  smpl(0, "Um Exemplo");
  reset();
  stream(1);

  /* Inicializacao */

  nodo = (Tnodo*) malloc(sizeof(Tnodo)*N);
  for(i = 0; i < N; i++){
    memset(fa_name, "\0", 5);
    sprintf(fa_name, "%d", i);
    nodo[i].id = facility(fa_name, 1);
    /* Aqui inicializa as variaveis locais */
  }

  /* Escalonamento de eventos */

  for(i=0; i<N; i++){
    schedule(test, 30.0, i);
  }

  schedule(fault, 50.0, 2); /* O nodo 2 vai falhar em 50.0 */
  schedule(repair, 80.0, 2);

  while(time() < 100){
    cause(&event, &token);
    switch(event){
    case test:
      if(status(nodo[token].id != 0)){
        break;
      }

      printf("O nodo %d vai testar no tempo %5.1f\n", token, time());
      schedule(test, 30.0, token);
      break;

    case fault:
      r = request(nodo[token].id, token, 0);
      if(r != 0){
        puts("Nao foi possivel falhar o nodo\n");
        exit(1);
      }
      printf("O nodo %d falhou no tempo %5.1f\n", token, time());
      break;

    case repair:
      printf("O nodo %d recuperou no tempo %5.1f\n", token, time());
      release(nodo[token].id, token);
      schedule(test, 30.0, token);
      break;

    }
  }
  return 0;
}
