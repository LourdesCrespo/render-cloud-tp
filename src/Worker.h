#ifndef WORKER_H
#define WORKER_H

#include "MessageQueue.h"
#include "PoolVRAM.h"

void ejecutarWorker(int idWorker, MessageQueue& mq, PoolVRAM& pool, int cantidad);

#endif// WORKER_H_INCLUDED
