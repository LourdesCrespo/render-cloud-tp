#ifndef POOL_VRAM_H
#define POOL_VRAM_H

#include <mutex>
#include "Job.h"
#include "Semaforo.h"
#include "logger.h"

class PoolVRAM {
private:
    int slotsOcupados;
    std::mutex mutexVram;

    Semaforo semaforoSlots;

public:
    PoolVRAM();

    void asignarVram(Job& job, int idWorker);
    void liberarVram(Job& job, int idWorker);
};

#endif // POOL_VRAM_H
