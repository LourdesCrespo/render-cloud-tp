#include "MessageQueue.h"

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

    hayJobs.wait(lock, [this]() {
        return !colaPremium.empty() || !colaFree.empty();
    });

    Job job;

    if (!colaPremium.empty()) {
        job = colaPremium.front();
        colaPremium.pop();
    } else {
        job = colaFree.front();
        colaFree.pop();
    }

    return job;
}

bool MessageQueue::estaVacia() {
    std::unique_lock<std::mutex> lock(mutexCola);
    return colaPremium.empty() && colaFree.empty();
}