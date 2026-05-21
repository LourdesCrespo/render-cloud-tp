#include <iostream>
#include <thread>
#include "MessageQueue.h"
#include "Producer.h"
#include "PoolVRAM.h"
#include "Worker.h"

int main() {
    MessageQueue messageQueue;
    PoolVRAM poolVram;

    // Productores
    Producer productor1(1, 5, messageQueue);
    Producer productor2(2, 5, messageQueue);

    std::thread hiloProductor1(&Producer::ejecutar, &productor1);
    std::thread hiloProductor2(&Producer::ejecutar, &productor2);

    // Workers
    std::thread hiloWorker1(ejecutarWorker, 1, std::ref(messageQueue), std::ref(poolVram), 5);
    std::thread hiloWorker2(ejecutarWorker, 2, std::ref(messageQueue), std::ref(poolVram), 5);

    hiloProductor1.join();
    hiloProductor2.join();

    hiloWorker1.join();
    hiloWorker2.join();

    std::cout << "Prueba finalizada con exito." << std::endl;

    return 0;
}
