#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "MessageQueue.h"
#include "PoolVRAM.h"
#include "Producer.h"
#include "Worker.h"
#include "logger.h"
#include <ctime>
//// Bandera global para controlar el ciclo de vida de los Workers concurrentes
std::atomic<bool> sistemaActivo(true);

// Función auxiliar para ejecutar el escenario con parámetros específicos
void ejecutarConfiguracionPrueba(int numProductores, int numConsumidores, int idEscenario, int totalJobs) {
    std::cout << "\n=======================================================" << std::endl;
    std::cout << "INICIANDO: Escenario " << idEscenario << " ("
              << numProductores << " Prod / " << numConsumidores << " Cons)" << std::endl;
    std::cout << "=======================================================" << std::endl;

    // Reiniciamos las estructuras de control para cada set de prueba
    sistemaActivo = true;
    jobsFinalizados = 0;
    MessageQueue messageQueue;
    PoolVRAM poolVram;

    std::vector<std::thread> hilosProductores;
    std::vector<std::thread> hilosConsumidores;

    // 1. Lanzar hilos Consumidores (Workers - Integrante 2)
    // Nota: Pasamos la cantidad de tareas estimadas o los dejamos sincronizados por la cola
    for (int i = 0; i < numConsumidores; ++i) {
        // Adaptamos a la firma de tu grupo: ejecutarWorker(id, mq, pool, cantidad)
        // Le pasamos un estimado o controlamos internamente por referencia
        hilosConsumidores.push_back(std::thread(ejecutarWorker, i + 1, std::ref(messageQueue), std::ref(poolVram), totalJobs));
    }

    // 2. Lanzar hilos Productores (Nodos API Gateway - Integrante 1)
    int jobsPorProductor = (numProductores > 0) ? (totalJobs / numProductores) : 0;

    std::vector<Producer*> productoresObjetos;
    for (int i = 0; i < numProductores; ++i) {
        // Instanciamos el objeto real del Integrante 1
        Producer* prod = new Producer(i + 1, jobsPorProductor, messageQueue);
        productoresObjetos.push_back(prod);

        // Lanzamos el hilo invocando el método ejecutar() del Producer
        hilosProductores.push_back(std::thread(&Producer::ejecutar, prod));
    }

    // 3. Esperar que los productores terminen de inyectar la carga
    for (auto& t : hilosProductores) {
        if (t.joinable()) t.join();
    }
    std::cout << "[INFO] --> Todos los hilos Productores finalizaron la carga." << std::endl;

    // 4. Tiempo de gracia para que los consumidores procesen el remanente en la cola de VRAM
    if (idEscenario == 1) {
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Más tiempo para la carga masiva
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    // 5. Apagado controlado: Cambiamos la bandera global
std::cout << "[INFO] --> Enviando señal de finalización a los Workers..." << std::endl;
sistemaActivo = false;
messageQueue.notificarFin();

    // 6. Sincronización final (.join) para asegurar recolección limpia de hilos
    for (auto& t : hilosConsumidores) {
        if (t.joinable()) t.join();
    }

    // Limpieza de memoria dinámica de los productores creados
    for (auto prod : productoresObjetos) {
        delete prod;
    }

    std::cout << "[OK] --> Escenario " << idEscenario << " completado sin fugas de hilos." << std::endl;
}

int main() {
    srand(time(nullptr));
    std::cout << "=======================================================" << std::endl;
    std::cout << "   SISTEMA DE GESTION DE RENDERIZADO - UNLa 2026       " << std::endl;
    std::cout << "   ORQUESTADOR DE PRUEBAS OBLIGATORIAS (INTEGRANTE 4)  " << std::endl;
    std::cout << "=======================================================" << std::endl;

    // --- CONFIGURACIÓN A: 1 Productor / 2 Consumidores ---
    // Escenario 1: Carga Masiva (1500 jobs) [Requerimiento de Cátedra]
    ejecutarConfiguracionPrueba(1, 2, 1, 1500);

    // Escenario 2: Vacuidad (0 jobs) [Verifica Espera Pasiva / 0% CPU]
    ejecutarConfiguracionPrueba(1, 2, 2, 0);

    // --- CONFIGURACIÓN B: 3 Productores / 1 Consumidor ---
    // Escenario 3: Saturación de Recursos (8 jobs Premium iniciales)
    ejecutarConfiguracionPrueba(3, 1, 3, 8);

    // --- CONFIGURACIÓN C: 3 Productores / 3 Consumidores ---
    // Escenario 4: Equidad (Anti-Starvation, validación de prioridad con retardo)
    ejecutarConfiguracionPrueba(3, 3, 4, 30);

    std::cout << "\n[FIN] Set de pruebas finalizado. Revise el archivo 'actividad.log'." << std::endl;
    return 0;
}

