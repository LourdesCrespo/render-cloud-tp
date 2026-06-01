#include "MessageQueue.h"
#include "Worker.h"

MessageQueue::MessageQueue() {
    flujoPremiumActivo = false;
}

void MessageQueue::encolar(Job job) {

    std::unique_lock<std::mutex> lock(mutexCola);

    job.estado = EN_COLA;

    if (job.prioridad == PREMIUM) {
        colaPremium.push(job);
    } else {
        colaFree.push(job);
    }

    hayJobs.notify_one();
}

Job MessageQueue::obtenerSiguiente() {

    std::unique_lock<std::mutex> lock(mutexCola);

    while (
        colaPremium.empty() &&
        colaFree.empty() &&
        sistemaActivo
    ) {
        hayJobs.wait(lock);
    }

    if (
        colaPremium.empty() &&
        colaFree.empty() &&
        !sistemaActivo
    ) {
        Job job;
        job.id = -1;
        return job;
    }

    Job job;

  // PRIORIDAD PREMIUM
    if (!colaPremium.empty()) {

        // Inicio del flujo exclusivo Premium si veníamos de procesar Free
        if (!flujoPremiumActivo) {
            inicioFlujoPremium = std::chrono::steady_clock::now();
            flujoPremiumActivo = true;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto tiempoPremium = std::chrono::duration_cast<std::chrono::milliseconds>(
            ahora - inicioFlujoPremium
        ).count();

        // Anti-Starvation basado en 5000 ms
        if (tiempoPremium >= 5000 && !colaFree.empty()) {

            job = colaFree.front();
            colaFree.pop();

            // CORRECCIÓN: En lugar de false, reiniciamos el reloj para la próxima ventana de 5 segundos
            inicioFlujoPremium = ahora;

            // Si la cola Premium queda vacía tras esto, el flag se apagará en la sección de abajo
            if(colaPremium.empty()) {
                flujoPremiumActivo = false;
            }

            return job;
        }

        // Si no se cumplió el tiempo o la cola Free estaba vacía, procesamos Premium normalmente
        job = colaPremium.front();
        colaPremium.pop();

        return job;
    }

    // Si no quedan tareas Premium en absoluto
    flujoPremiumActivo = false;

    job = colaFree.front();
    colaFree.pop();

    return job;
}

bool MessageQueue::estaVacia() {

    std::unique_lock<std::mutex> lock(mutexCola);

    return colaPremium.empty() &&
           colaFree.empty();
}

void MessageQueue::notificarFin() {
    hayJobs.notify_all();
}
