#include "Worker.h"
#include <iostream>
#include <thread>
#include <chrono>

std::atomic<int> jobsFinalizados(0);

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad) {

    while (sistemaActivo || !mq.estaVacia()) {

        Job miJob = mq.obtenerSiguiente();

        if (miJob.id == -1) {
            break;
        }

        std::cout << "[Worker "
                  << idWorker
                  << "] tomo Job "
                  << miJob.id
                  << std::endl;

        pool.asignarVram(miJob, idWorker);

        // Tiempo mínimo de procesamiento requerido
        std::this_thread::sleep_for(
            std::chrono::milliseconds(600));

        pool.liberarVram(miJob, idWorker);

        jobsFinalizados++;

        std::cout << "[INFO] Total Jobs Finalizados: "
                  << jobsFinalizados.load()
                  << std::endl;
    }

    std::cout << "[Worker "
              << idWorker
              << "] finalizado."
              << std::endl;
}
