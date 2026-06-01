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
    std::lock_guard<std::mutex> lock(mutexIds);
    contadorIds++;
    return contadorIds;
}

Prioridad generarPrioridad() {
    int valor = rand() % 2;

    if (valor == 0) {
        return FREE;
    }

    return PREMIUM;
}

void Producer::ejecutar() {
    for (int i = 0; i < cantidadJobs; i++) {

        Job job;
        job.id = generarIdUnico();
        job.prioridad = generarPrioridad();
        job.estado = CREADO;

        registrarEvento(job, "CREADO");

        std::cout << "[Productor " << idProductor
                  << "] Job creado: "
                  << job.id
                  << " Prioridad: "
                  << job.prioridad
                  << std::endl;

        messageQueue.encolar(job);

        registrarEvento(job, "EN_COLA");

        std::cout << "[Productor " << idProductor
                  << "] Job en cola: "
                  << job.id
                  << std::endl;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }
}
