#include "PoolVRAM.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "logger.h"

PoolVRAM::PoolVRAM()
    : semaforoSlots(5)
{
    slotsOcupados = 0;
}

void PoolVRAM::asignarVram(Job& job, int idWorker) {

    // Espera hasta que haya lugar en la VRAM
    semaforoSlots.wait();

    {
        // Se actualizan los datos del job y la cantidad de slots ocupados
        std::unique_lock<std::mutex> lock(mutexVram);

        slotsOcupados++;
        job.estado = ASIGNADO_VRAM;

        registrarEvento(job, "ASIGNADO_VRAM");

        std::cout << "[VRAM] Worker "
                  << idWorker
                  << " asigno Job "
                  << job.id
                  << " | Slots: "
                  << slotsOcupados
                  << "/5"
                  << std::endl;
    }

    // Simula el tiempo de carga en VRAM
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
}

void PoolVRAM::liberarVram(Job& job, int idWorker) {

    {
        // Se libera el slot que estaba usando el job
        std::unique_lock<std::mutex> lock(mutexVram);

        slotsOcupados--;
        job.estado = FINALIZADO;

        registrarEvento(job, "FINALIZADO");

        std::cout << "[VRAM] Worker "
                  << idWorker
                  << " libero Job "
                  << job.id
                  << " | Slots: "
                  << slotsOcupados
                  << "/5"
                  << std::endl;
    }

    // Se avisa que hay un lugar libre
    semaforoSlots.signal();

    // espera antes de la siguiente liberacion
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}
