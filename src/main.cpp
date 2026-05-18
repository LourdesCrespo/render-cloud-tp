#include <iostream>
#include <thread>
#include <vector>
#include "MessageQueue.h"
#include "Producer.h"

int main() {
    MessageQueue messageQueue;

    Producer productor1(1, 5, messageQueue);
    Producer productor2(2, 5, messageQueue);

    std::thread hiloProductor1(&Producer::ejecutar, &productor1);
    std::thread hiloProductor2(&Producer::ejecutar, &productor2);

    hiloProductor1.join();
    hiloProductor2.join();

    std::cout << "Produccion finalizada." << std::endl;

    while (!messageQueue.estaVacia()) {
        Job job = messageQueue.obtenerSiguiente();

        std::cout << "Job obtenido de la cola: "
                  << job.id
                  << " Prioridad: "
                  << job.prioridad
                  << std::endl;
    }

    return 0;
}