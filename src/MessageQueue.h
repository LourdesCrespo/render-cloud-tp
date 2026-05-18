#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "Job.h"

class MessageQueue {
private:
    std::queue<Job> colaPremium;
    std::queue<Job> colaFree;

    std::mutex mutexCola;
    std::condition_variable hayJobs;

public:
    void encolar(Job job);
    Job obtenerSiguiente();
    bool estaVacia();
};

#endif