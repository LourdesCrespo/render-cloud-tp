#include "MessageQueue.h"
#include "Worker.h"

MessageQueue::MessageQueue() {
    flujoPremiumActivo = false;
}

void MessageQueue::encolar(Job job) {
    std::unique_lock<std::mutex> lock(mutexCola);

    job.estado = EN_COLA;

    // Se guarda el job en la cola que corresponde segun su prioridad
    if (job.prioridad == PREMIUM) {
        colaPremium.push(job);
    } else {
        colaFree.push(job);
    }

    // Se avisa que hay un nuevo job disponible
    hayJobs.notify_one();
}

Job MessageQueue::obtenerSiguiente() {
    std::unique_lock<std::mutex> lock(mutexCola);

    // Si no hay jobs, el worker queda esperando
    while (colaPremium.empty() && colaFree.empty() && sistemaActivo) {
        hayJobs.wait(lock);
    }

    // Si el sistema termino y no quedan jobs, se devuelve un job invalido
    if (colaPremium.empty() && colaFree.empty() && !sistemaActivo) {
        Job jobInvalido;
        jobInvalido.id = -1;
        return jobInvalido;
    }

    Job job;

    // Primero se intenta tomar un job Premium
    if (!colaPremium.empty()) {

        if (!flujoPremiumActivo) {
            inicioFlujoPremium = std::chrono::steady_clock::now();
            flujoPremiumActivo = true;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto tiempoPremium = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - inicioFlujoPremium).count();

        // Si los Premium vienen ocupando mucho tiempo, se deja pasar un Free
        if (tiempoPremium >= 5000 && !colaFree.empty()) {
            job = colaFree.front();
            colaFree.pop();

            // Se reinicia el tiempo para volver a controlar la espera
            inicioFlujoPremium = ahora;

            if (colaPremium.empty()) {
                flujoPremiumActivo = false;
            }

            return job;
        }

        job = colaPremium.front();
        colaPremium.pop();

        return job;
    }

    // Si no hay Premium, se toma un job Free
    flujoPremiumActivo = false;

    job = colaFree.front();
    colaFree.pop();

    return job;
}

bool MessageQueue::estaVacia() {
    std::unique_lock<std::mutex> lock(mutexCola);

    return colaPremium.empty() && colaFree.empty();
}

void MessageQueue::notificarFin() {
    std::unique_lock<std::mutex> lock(mutexCola);

    // Se despiertan los workers para que puedan terminar
    hayJobs.notify_all();
}
