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
  int *stats; /* Identifica o status do nodo que ele testou */
}Tnodo;

Tnodo *nodo;

int main(int argc, char *argv[]){
  static int N, /* Numero de nodos */
    token,
    event,
    teste,
    next_node,
    r, i, j;

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
    nodo[i].stats = (int*) malloc(sizeof(int)*N);
    nodo[i].stats[i] = NULL;
    /* Aqui inicializa as variaveis locais */
  }

  /* Escalonamento de eventos */

  for(i=0; i<N; i++){
    schedule(test, 30.0, i);
  }

  schedule(fault, 50.0, 2); /* O nodo 2 vai falhar em 50.0 */
  schedule(fault, 50.0, 3);
  schedule(fault, 50.0, 4);
  schedule(repair, 100.0, 3);


  while(time() < 200){
    cause(&event, &token);
    switch(event){

    case test:

      if(status(nodo[token].id) != 0)
        break;

      i = 1;
      while(status(nodo[(token+i)%N].id)){
        nodo[token].stats[(token+i)%N] = status(nodo[(token+i)%N].id);
        i++;
      }

      /* Grava qual o próximo nodo nao falho */
      next_node = (token+i)%N;
      nodo[token].stats[next_node] = status(nodo[next_node].id);

      /* Assimila a informação do nodo não falho encontrado */
      j = next_node+1;
      while(j%N != token){
        nodo[token].stats[j%N] = nodo[next_node].stats[j%N];
        j++;
      }

      schedule(test, 30.0, token);

      /*-------- Imprime os vetores de estado --------*/
      printf("Vetor de estados nodo %d:\n ", token);
      for(j = 0; j < N; j++)
        printf("%d|", j);

      printf("\n[");

      for(j = 0; j < N; j++)
        if(j != token)
          printf("%d|", nodo[token].stats[j]);
        else
          printf("-|");

      printf("]\n\n");
      /*----------------------------------------------*/

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
