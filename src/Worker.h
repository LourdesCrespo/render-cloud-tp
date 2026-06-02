#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include "MessageQueue.h"
#include "PoolVRAM.h"

extern std::atomic<bool> sistemaActivo;
extern std::atomic<int> jobsFinalizados;

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad);

#endif WORKER_H

