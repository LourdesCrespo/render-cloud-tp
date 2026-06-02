#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "Job.h"

class MessageQueue {
private:
    std::queue<Job> colaPremium;
    std::queue<Job> colaFree;

    std::mutex mutexCola;
    std::condition_variable hayJobs;

    std::chrono::steady_clock::time_point inicioFlujoPremium;
    bool flujoPremiumActivo;

public:
    MessageQueue();

    void encolar(Job job);
    Job obtenerSiguiente();
    bool estaVacia();

    void notificarFin();
};

#endif
