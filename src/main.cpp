#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>
#include "MessageQueue.h"
#include "PoolVRAM.h"
#include "Producer.h"
#include "worker.h"
#include "logger.h"

// Control de los workers
std::atomic<bool> sistemaActivo(true);

// Jobs terminados
std::atomic<int> jobsFinalizados(0);

void ejecutarConfiguracionPrueba(int numProductores, int numConsumidores, int idEscenario, int totalJobs) {
    std::cout << "\n=======================================================" << std::endl;
    std::cout << "INICIANDO: Escenario " << idEscenario << " ("
              << numProductores << " Prod / " << numConsumidores << " Cons)" << std::endl;
    std::cout << "=======================================================" << std::endl;

    sistemaActivo = true;
    jobsFinalizados.store(0);

    MessageQueue messageQueue;
    PoolVRAM poolVram;

    std::vector<std::thread> hilosProductores;
    std::vector<std::thread> hilosConsumidores;

    // Crear workers
    for (int i = 0; i < numConsumidores; ++i) {
        hilosConsumidores.push_back(
            std::thread(ejecutarWorker, i + 1, std::ref(messageQueue), std::ref(poolVram), totalJobs)
        );
    }

    // Escenario 3: todos Premium
    if (idEscenario == 3) {
        for (int i = 0; i < totalJobs; i++) {
            Job job;
            job.id = i + 1;
            job.prioridad = PREMIUM;
            job.estado = CREADO;
            registrarEvento(job, "CREADO");
            messageQueue.encolar(job);
            registrarEvento(job, "EN_COLA");
            std::cout << "[Escenario 3] Job Premium " << job.id << " en cola" << std::endl;
        }
    } else {
        //Productores normales
        int jobsPorProductor = (numProductores > 0) ? (totalJobs / numProductores) : 0;

            std::vector<Producer*> productoresObjetos;
            for (int i = 0; i < numProductores; ++i) {
                Producer* prod = new Producer(i + 1, jobsPorProductor, messageQueue);
                productoresObjetos.push_back(prod);
                hilosProductores.push_back(std::thread(&Producer::ejecutar, prod));
            }

            for (auto& t : hilosProductores) {
                if (t.joinable()) t.join();
            }

            for (auto prod : productoresObjetos) {
                delete prod;
            }
        }

        std::cout << "\n[INFO] --> Todos los Productores terminaron de cargar jobs." << std::endl;

        // Espera para terminar los jobs
        if (idEscenario == 1) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

        std::cout << "[INFO] --> Enviando señal de finalizacion a los Workers..." << std::endl;
        sistemaActivo = false;
        messageQueue.notificarFin();

        for (auto& t : hilosConsumidores) {
            if (t.joinable()) t.join();
        }

        std::cout << "\n====================================================" << std::endl;
        std::cout << "ESCENARIO " << idEscenario << " FINALIZADO" << std::endl;
        std::cout << "Jobs procesados: "
              << jobsFinalizados.load()
              << "/" << totalJobs
              << std::endl;
        std::cout << "====================================================" << std::endl;
    }

    int main() {
        srand(time(nullptr));

    std::cout << "\n====================================================" << std::endl;
    std::cout << "TP SISTEMA DE RENDERIZADO EN LA NUBE" << std::endl;
    std::cout << "====================================================" << std::endl;


    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 1 - CARGA MASIVA (1500 JOBS)" << std::endl;
    std::cout << "CONFIGURACION A (1 Productor - 2 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(1, 2, 1, 1500);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 1 - CARGA MASIVA (1500 JOBS)" << std::endl;
    std::cout << "CONFIGURACION B (3 Productores - 1 Consumidor)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 1, 1, 1500);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 1 - CARGA MASIVA (1500 JOBS)" << std::endl;
    std::cout << "CONFIGURACION C (3 Productores - 3 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 3, 1, 1500);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 2 - VACUIDAD (0 JOBS)" << std::endl;
    std::cout << "CONFIGURACION A (1 Productor - 2 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(1, 2, 2, 0);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 2 - VACUIDAD (0 JOBS)" << std::endl;
    std::cout << "CONFIGURACION B (3 Productores - 1 Consumidor)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 1, 2, 0);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 2 - VACUIDAD (0 JOBS)" << std::endl;
    std::cout << "CONFIGURACION C (3 Productores - 3 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 3, 2, 0);



    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 3 - SATURACION (8 JOBS PREMIUM)" << std::endl;
    std::cout << "CONFIGURACION A (1 Productor - 2 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(1, 2, 3, 8);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 3 - SATURACION (8 JOBS PREMIUM)" << std::endl;
    std::cout << "CONFIGURACION B (3 Productores - 1 Consumidor)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 1, 3, 8);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 3 - SATURACION (8 JOBS PREMIUM)" << std::endl;
    std::cout << "CONFIGURACION C (3 Productores - 3 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 3, 3, 8);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 4 - ANTI STARVATION" << std::endl;
    std::cout << "CONFIGURACION A (1 Productor - 2 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(1, 2, 4, 30);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 4 - ANTI STARVATION" << std::endl;
    std::cout << "CONFIGURACION B (3 Productores - 1 Consumidor)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 1, 4, 30);

    std::cout << "\n====================================================" << std::endl;
    std::cout << "ESCENARIO 4 - ANTI STARVATION" << std::endl;
    std::cout << "CONFIGURACION C (3 Productores - 3 Consumidores)" << std::endl;
    std::cout << "====================================================" << std::endl;
    ejecutarConfiguracionPrueba(3, 3, 4, 30);

    return 0;
}
