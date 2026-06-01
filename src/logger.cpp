#include "logger.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

std::mutex logMutex;

void registrarEvento(const Job& job, const std::string& evento) {
    std::lock_guard<std::mutex> lock(logMutex);


   std::ofstream archivo("actividad.log", std::ios::app);
    if (archivo.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

        // Formato requerido: [Timestamp] - Job ID - Prioridad - Evento
        archivo << "[" << ss.str() << "] - "
                << "Job ID: " << job.id << " - "
                << "Prioridad: " << (job.prioridad == PREMIUM ? "PREMIUM" : "FREE") << " - "
                << "Evento: " << evento << "\n";

        archivo.close();
    }
}
