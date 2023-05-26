#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

//
// TODO: Definição dos semáforos (variáveis precisam ser globais)
//
sem_t *chopstick_semaphores;  // Semáforos para controlar o acesso aos palitos

// lista quem está de posse de um chopstick
int *chopstick_use;

// número de filósofos
int N_FILOS;

// protótipos das funções
void * filosofo(void *);
void pegar(int, int);
void liberar(int, int);
int gera_rand(int);

int main(int argc, char ** argv)
{
    // threads dos filósofos
    pthread_t * tids;

    long i;

    srand(time(NULL));

    if ( argc < 2 )
    {
        printf("Usage: %s num_filosofos\n", argv[0]);
        return 0;
    }
    
    // número de filósofos
    N_FILOS = atoi(argv[1]);
    
    // gerando uma lista de threads de filósofos
    tids = malloc(N_FILOS * sizeof(pthread_t));

    // gerando uma lista de uso dos palitos
    chopstick_use = malloc(N_FILOS * sizeof(int));
    
    // um palito está livre se estiver -1
    for (i = 0; i < N_FILOS; i++)
    {
        chopstick_use[i] = -1;
    }

    //
    // TODO: Criação dos semáforos (aqui é quando define seus valores)
    //
    chopstick_semaphores = malloc(N_FILOS * sizeof(sem_t));
    for (i = 0; i < N_FILOS; i++)
    {
        sem_init(&chopstick_semaphores[i], 0, 1);  // Inicializa cada semáforo com valor 1 (liberado)
    }
 
    // iniciando as threads dos filósofos
    for (i = 0; i < N_FILOS; i++)
    {
        pthread_create(&tids[i], NULL, filosofo, (void *)i);
    }
    
    // aguardando as threads dos filósofos terminarem
    for (i = 0; i < N_FILOS; i++)
    {
        pthread_join(tids[i], NULL);
    }
    
    //
    // TODO: Excluindo os semáforos
    // 
    for (i = 0; i < N_FILOS; i++)
    {
        sem_destroy(&chopstick_semaphores[i]);  // Destroi cada semáforo
    }
    free(chopstick_semaphores);

    // liberando a memória alocada
    free(tids);
    free(chopstick_use);

    return 0;
}

void * filosofo(void * id)
{
    // convertendo o ID do filósofo para int
    long i = (long)id;
    
    printf("\t> Filosofo %d pensando\n",i);
    usleep(gera_rand(1000000));

    // ordem dos palitos depende do ID  
    int c1, c2;

    // OBS: alterando a ordem de pegar o palito para evitar deadlock
    if (i%2 == 0) // com base no ID do filósofo (par ou ímpar)
    {
        c1 = i;             // esquerda
        c2 = (i+1)%N_FILOS; // direita
    }
    else
    {
        c1 = (i+1)%N_FILOS; // direita
        c2 = i;             // esquerda 
    }

    //
    // TODO: precisa garantir que mais de um filósofo não pegue o mesmo
    // palito simultaneamente
    //

    sem_wait(&chopstick_semaphores[i]);  // Espera até que o palito esteja disponível
    sem_wait(&chopstick_semaphores[(i + 1) % N_FILOS]);
    
    pegar(i, c1);
    pegar(i, c2);
    
    printf("\t> Filosofo %d comendo\n",i);
    usleep(gera_rand(1000000));
    
    //
    // TODO: precisa garantir que os filósofos liberem os palitos
    // após usar
    //

    sem_post(&chopstick_semaphores[i]);  // Sinaliza que o palito foi liberado
    sem_post(&chopstick_semaphores[(i + 1) % N_FILOS]);

    liberar(i, c1);
    liberar(i, c2);
}

// filósofo 'i' quer pegar o palito definido por 'num'
void pegar(int i, int num)
{
    
    if (chopstick_use[num] != -1)
    {
        printf("===== ALERTA DO FILOSOFO %d =====\n===== CHOPSTICK[%d] EM USO POR %d =====\n",
                i,num,chopstick_use[num]);
    }
    chopstick_use[num] = i;
    printf("+ Filosofo %d pegou o chopstick[%d]\n",i,num);
}

// filósofo 'i' quer liberar o palito definido por 'num'
void liberar(int i, int num)
{
    chopstick_use[num] = -1;
    printf("- Filosofo %d liberou o chopstick[%d]\n",i,num);
}

int gera_rand(int limit)
{
    // 0 a (limit -1)
    return rand()%limit;
}

