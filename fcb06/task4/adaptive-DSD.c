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
  int stats; /* Identifica o status do nodo que ele testou */
}Tnodo;

Tnodo *nodo;

int main(int argc, char *argv[]){
  static int N, /* Numero de nodos */
    token,
    event,
    teste,
    r, i;

  static char fa_name[5]; /* Facility Name */
  char *fail_code[2] = {"Sem falha", "Falho"};

  if(argc != 2){
    printf("USO: %s num_nodos\n", argv[0]);
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
    nodo[i].stats = -1;
    /* Aqui inicializa as variaveis locais */
  }

  /* Escalonamento de eventos */

  for(i=0; i<N; i++){
    schedule(test, 30.0, i);
  }

  schedule(fault, 50.0, 2); /* O nodo 2 vai falhar em 50.0 */
  schedule(fault, 50.0, 3);
  schedule(fault, 50.0, 4);
  schedule(repair, 100.0, 2);


  while(time() < 200){
    cause(&event, &token);
    switch(event){
    case test:

      if(status(nodo[token].id) != 0)
        break;

      teste = status(nodo[(token+1)%N].id);
      i = 1;
      while(teste){
        teste = status(nodo[(token+i)%N].id);
        if(!teste == 0){
          nodo[token].stats = (token+i)%N;
          i++;
        }
      }

      printf("%5.1f - O nodo %d testou o nodo %d. Resultado: %s\n",
             time(), token, (token+i)%N,
             fail_code[teste], nodo[token].stats);
      schedule(test, 30.0, token);

      break;

    case fault:
      r = request(nodo[token].id, token, 0);
      if(r != 0){
        puts("Nao foi possivel falhar o nodo\n");
        exit(1);
      }
      printf("%5.1f - O nodo %d falhou\n", time(), token);
      break;

    case repair:
      printf("%5.1f - O nodo %d recuperou\n", time(), token);
      release(nodo[token].id, token);
      schedule(test, 30.0, token);
      break;

    }
  }
  return 0;
}
