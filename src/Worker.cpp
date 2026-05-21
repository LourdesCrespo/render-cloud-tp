#include "Worker.h"
#include <iostream>
#include <thread>
#include <chrono>

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad) {
    for (int i = 0; i < cantidad; i++) {

        Job miJob = mq.obtenerSiguiente();

        std::cout << "[Worker " << idWorker
                  << "] tomo Job " << miJob.id << std::endl;

        pool.asignarVram(miJob, idWorker);

        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        pool.liberarVram(miJob, idWorker);
    }
}
