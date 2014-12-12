#include <iostream>
#include <iomanip>
#include <pthread.h>//require -lpthread on build options / linker settings
#include <semaphore.h>
#include <stdlib.h> //for rand()

using namespace std;

const int BUFFER_ELEMENTS = 16;
const int TOTAL_DATA = 512;
void *producer(void *);
void *consumer(void *);

//Estructura compartida por los hilos
struct
{
    int buffer[BUFFER_ELEMENTS]; //matriz de datos
    int producerIndex, consumerIndex;//indices del productor y del consumidor
    pthread_mutex_t mutualExclusion;//exclusión mutua
    sem_t emptySemaphoreCounter;//semáforo contador de elementos vacios
    sem_t fullSemaphoreCounter;//semáfoto contador de elementos llenos
} bufferStructure;

int main()
{
    pthread_t producerThread, consumerThread;//hilos productor y consumidor

    //Iniciar la estructura bufferStructure
    bufferStructure.producerIndex =bufferStructure.consumerIndex = 0;
    pthread_mutex_init(&bufferStructure.mutualExclusion, 0);
    sem_init(&bufferStructure.emptySemaphoreCounter, 0, BUFFER_ELEMENTS);
    sem_init(&bufferStructure.fullSemaphoreCounter, 0, 0);

    //Crear los hilos
    pthread_create(&producerThread, 0, producer, 0);
    pthread_create(&consumerThread, 0, consumer, 0);

    //Esperar a que los hilos trabajadores terminen
    pthread_join(producerThread, 0);
    pthread_join(consumerThread, 0);
}

//Productor
void *producer(void *arg)
{
    int i;
    for(i = 0; i < TOTAL_DATA; i++)
    {
        sem_wait(&bufferStructure.emptySemaphoreCounter);//decrementar el contador de vacíos
        pthread_mutex_lock(&bufferStructure.mutualExclusion);//echar el cerrojo

        //Búfer circular: añadir un elemento
        bufferStructure.buffer[bufferStructure.producerIndex] = rand();
        bufferStructure.producerIndex = (bufferStructure.producerIndex + 1) % (BUFFER_ELEMENTS);
        cout << "producer buffer < --" << setw(6) << i << endl;

        pthread_mutex_unlock(&bufferStructure.mutualExclusion);//quitar el cerrojo
        sem_post(&bufferStructure.fullSemaphoreCounter);//incrementar el contador de llenos
    }
    return 0;
}

//Consumidor
void *consumer(void *arg)
{
    int i, a;
    for(i = 0; i < TOTAL_DATA; i++)
    {
        sem_wait(&bufferStructure.fullSemaphoreCounter);//decrementar el contador de llenos
        pthread_mutex_lock(&bufferStructure.mutualExclusion);//echar el cerrojo

        //Búfer circular: recoger un elemento
        a = bufferStructure.buffer[bufferStructure.consumerIndex];
        bufferStructure.consumerIndex = (bufferStructure.consumerIndex + 1) % (BUFFER_ELEMENTS);
        cout << "consumer buffer < --" << setw(6) << a << endl;

        pthread_mutex_unlock(&bufferStructure.mutualExclusion);//quitar el cerrojo
        sem_post(&bufferStructure.fullSemaphoreCounter);//incrementar el contador de vacios
    }
    return 0;
}
