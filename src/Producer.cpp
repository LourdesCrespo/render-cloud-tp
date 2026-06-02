#include "Producer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "logger.h"

static int contadorIds = 0;
static std::mutex mutexIds;

Producer::Producer(int idProductor, int cantidadJobs, MessageQueue& messageQueue)
    : idProductor(idProductor),
      cantidadJobs(cantidadJobs),
      messageQueue(messageQueue) {}

int generarIdUnico() {

    // Genera ids sin repetir entre productores
    std::lock_guard<std::mutex> lock(mutexIds);

    contadorIds++;
    return contadorIds;
}

Prioridad generarPrioridad() {

    // Prioridad aleatoria: Premium o Free
    int valor = rand() % 2;

    if (valor == 0) {
        return FREE;
    }

    return PREMIUM;
}

void Producer::ejecutar() {

    // Genera la cantidad de jobs asignados al productor
    for (int i = 0; i < cantidadJobs; i++) {

        Job job;

        job.id = generarIdUnico();
        job.prioridad = generarPrioridad();
        job.estado = CREADO;

        // Registra la creacion del job en el log
        registrarEvento(job, "CREADO");

        std::cout << "[PRODUCTOR " << idProductor
                  << "] creo Job "
                  << job.id
                  << " ("
                  << (job.prioridad == PREMIUM ? "PREMIUM" : "FREE")
                  << ")"
                  << std::endl;

        // Inserta el job en la Message Queue
        messageQueue.encolar(job);

        // Registra el cambio de estado en el log
        registrarEvento(job, "EN_COLA");

        std::cout << "[PRODUCTOR " << idProductor
                  << "] envio Job "
                  << job.id
                  << " a la cola"
                  << std::endl;

        // Retardo de 100 ms entre ingresos a la cola
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
