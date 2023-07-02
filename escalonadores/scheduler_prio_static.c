#include <stdio.h>
#include <stdlib.h>

#include "queue.h" // contem funções uteis para filas
#include "proc.h"  // possui as funções dos processos
#include "stats.h" // possui as funções de estatisticas 
#include "utils.h" // possui funções uteis 

// Utilizando as variáveis globais definidas no 'main'
extern struct queue * ready;    // fila de aptos
extern struct queue * ready2;   // segunda fila de aptos
extern struct queue * blocked;  // fila de bloqueados
extern struct queue * finished; // fila de finalizados
// NOTE: essa fila de finalizados é utilizada apenas para
// as estatisticas finais

// variavel global que indica o tempo maximo que um processo pode executar ao todo
extern int MAX_TIME;

struct proc * select_process(struct queue * ready, struct queue * ready2){
    int num = rand() % 100; // seleciona um número entre 0 e 99
    struct proc * p;
    
    // regras para selecionar processos:
    //  fila ready: 80% de probabilidade
    //  fila ready2: 20% de probabilidade
    if (num < 80 && !isempty(ready)){
        p =  dequeue(ready);
    }
    else if (!isempty(ready2)){
        p = dequeue(ready2);
    }
    else if (!isempty(ready)) {
        p = dequeue(ready);
    }
    else {
        p = NULL;
    }

    return p;
}

struct proc * scheduler(struct proc * current)
{
    struct proc * selected; 

    if (current != NULL){
        switch (current -> state){
            case READY:
                if (current -> queue == 1){
                    enqueue(ready, current);
                } else{
                    enqueue(ready2, current);
                }
                count_ready_in(current);
                break;
            case BLOCKED:
                enqueue(blocked, current);
                count_blocked_in(current);
                break;

            case FINISHED:
                enqueue(finished, current);
                count_finished_in(current);
                break;

            default:
                printf("@@ ERRO no estado de saída do processo %d\n", current->pid);
        }
    }

    if (isempty(ready) && isempty(ready2)){
        return NULL;
    }

    selected = select_process(ready, ready2);

    count_ready_out(selected);
    selected->state = RUNNING;

    return selected;
}


