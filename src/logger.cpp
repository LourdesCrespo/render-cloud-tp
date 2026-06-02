#include "logger.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

// Evita que varios hilos escriban al mismo tiempo
std::mutex logMutex;

void registrarEvento(const Job& job, const std::string& evento) {

    // Protege el acceso al archivo de log
    std::lock_guard<std::mutex> lock(logMutex);

    std::ofstream archivo("actividad.log", std::ios::app);

    if (archivo.is_open()) {

        // Obtiene la fecha y hora actual
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

        // Guarda la informacion del evento en el archivo
        archivo << "[" << ss.str() << "] - "
                << "Job ID: " << job.id << " - "
                << "Prioridad: "
                << (job.prioridad == PREMIUM ? "PREMIUM" : "FREE")
                << " - "
                << "Evento: " << evento
                << "\n";

        archivo.close();
    }
}
