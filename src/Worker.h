#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include "MessageQueue.h"
#include "PoolVRAM.h"

// Permite al Worker conocer el estado de apagado controlado definido en el Main
extern std::atomic<bool> sistemaActivo;

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad);

#endif // WORKER_H
