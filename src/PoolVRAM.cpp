#include "PoolVRAM.h"
#include <iostream>
#include <thread>
#include <chrono>

PoolVRAM::PoolVRAM() {
    slotsOcupados = 0;
}

void PoolVRAM::asignarVram(Job& job, int idWorker) {
    std::unique_lock<std::mutex> lock(mutexVram);

    while (slotsOcupados >= 5) {
        cvSlots.wait(lock);
    }

    slotsOcupados++;
    job.estado = ASIGNADO_VRAM;

    std::cout << "[VRAM] Worker " << idWorker
              << " asigno Job " << job.id
              << " a VRAM. Slots: "
              << slotsOcupados << "/5" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(450));
}

void PoolVRAM::liberarVram(Job& job, int idWorker) {
    std::unique_lock<std::mutex> lock(mutexVram);

    slotsOcupados--;
    job.estado = FINALIZADO;

    std::cout << "[VRAM] Worker " << idWorker
              << " libero Job " << job.id
              << " de VRAM. Slots: "
              << slotsOcupados << "/5" << std::endl;

    cvSlots.notify_one();

    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}
