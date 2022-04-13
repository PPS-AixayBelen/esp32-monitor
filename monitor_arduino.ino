#include <pthread.h>
 
void *printThreadId(void *threadid) {
   Serial.println((int)threadid);
}
 
void setup() {
 
   Serial.begin(115200);
   main();
 
}


 
void loop() {}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define NumRec 10
#define NumProc 5

pthread_mutex_t mutex;
pthread_cond_t espera;
int recdis = NumRec;

void pidoRec(int n, int id){
//pide lock del semaforo
    pthread_mutex_lock(&mutex);
    //mientras la cant de recursos solicitados sea mayor a la cant disponible
    while(recdis < n){
        //el hilo se va a dormir
        Serial.println( (String) "Trabajador " + id +" pide " + n + " -- espera \n");
     
        
        pthread_cond_wait(&espera, &mutex);
    }

    //disminuye la cant de recursos disponibles
    recdis = recdis -n;
    Serial.println( (String) "Trabajador " + id + " pide " + n + " -- lo obtiene \n");

    //devuelve el mutex
    pthread_mutex_unlock(&mutex);
}

void devuelvoRec(int n, int id){
    pthread_mutex_lock(&mutex);

    //aumenta cant de rec disponible 
    recdis = recdis + n;
    Serial.println( (String) "Trabajador " + id + " devuelve " + n + "\n");

    //notifyAll() :v
    pthread_cond_broadcast(&espera);
    pthread_mutex_unlock(&mutex);

}

void *cliente(void *arg){

    //id del cliente
    int miid = (int) arg;
    int d, i;
    //el cliente pide y devuelve recursos 10 veces
    for (i = 0; i < 10; i++){
        //cant de recursos es random
        d = rand() % 10;
        pidoRec(d, miid);
        //usleep(rand() % 10* 10);
        devuelvoRec(d, miid);
    }

    //y el hilo se muere
    pthread_exit(NULL);
}

int main (){

    int i;
    pthread_attr_t atrib;
    pthread_t c[NumProc];

    pthread_attr_init(&atrib);

    //battle royale de hilos por los recursos(?)
   // pthread_attr_setscope(&atrib, PTHREAD_SCOPE_SYSTEM);

    //el scheduling es round robin 
   // i = pthread_attr_setschedpolicy(&atrib, SCHED_RR);

    pthread_mutex_init(&mutex, NULL);
    //inicializa la cola de condicion donde esperan los hilos que no consiguen el mutex
    pthread_cond_init(&espera, NULL);

    //inicializa 5 clientes
    for (i = 0; i < 5; i++)
        pthread_create(&c[i], &atrib, cliente, (void *)i);
    
    //espera que terminen
    for(i = 0; i < 5; i++)
        pthread_join(c[i], NULL);
}
