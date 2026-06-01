#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include "MessageQueue.h"
#include "PoolVRAM.h"

// Bandera global para apagado controlado
extern std::atomic<bool> sistemaActivo;

// Contador global de jobs finalizados
extern std::atomic<int> jobsFinalizados;

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad);

#endif // WORKER_H

