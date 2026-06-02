#include "worker.h"
#include <iostream>
#include <thread>
#include <chrono>

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad) {

    // El worker sigue trabajando mientras el sistema este activo
    // o queden jobs pendientes en la cola
    while (sistemaActivo || !mq.estaVacia()) {

        Job miJob = mq.obtenerSiguiente();

        // Si recibe un job invalido termina su ejecucion
        if (miJob.id == -1) {
            break;
        }

        std::cout << "[WORKER " << idWorker
                  << "] tomo Job "
                  << miJob.id
                  << std::endl;

        // Intenta asignar el job a un slot de VRAM
        pool.asignarVram(miJob, idWorker);

        // El job debe permanecer al menos 600 ms en VRAM
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        // Libera el slot una vez finalizado el procesamiento
        pool.liberarVram(miJob, idWorker);

        // Actualiza el contador global de jobs terminados
        jobsFinalizados++;

        std::cout << "[INFO] Jobs finalizados: "
                  << jobsFinalizados.load()
                  << std::endl;
    }

    std::cout << "[WORKER "
              << idWorker
              << "] finalizado."
              << std::endl;
}
