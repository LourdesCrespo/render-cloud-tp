#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <mutex>
#include <condition_variable>

class Semaforo {
private:
    int contador;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Semaforo(int valorInicial) : contador(valorInicial) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);

        while (contador == 0) {
            cv.wait(lock);
        }

        contador--;
    }

    void signal() {
        std::unique_lock<std::mutex> lock(mtx);

        contador++;

        cv.notify_one();
    }
};

#endif
