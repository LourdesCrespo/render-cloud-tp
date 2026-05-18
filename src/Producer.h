#ifndef PRODUCER_H
#define PRODUCER_H

#include "MessageQueue.h"

class Producer {
private:
    int idProductor;
    int cantidadJobs;
    MessageQueue& messageQueue;

public:
    Producer(int idProductor, int cantidadJobs, MessageQueue& messageQueue);

    void ejecutar();
};

#endif