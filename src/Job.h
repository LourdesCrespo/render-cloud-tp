#ifndef JOB_H
#define JOB_H

enum Prioridad {
    FREE = 0,
    PREMIUM = 1
};

enum EstadoJob {
    CREADO,
    EN_COLA,
    ASIGNADO_VRAM,
    FINALIZADO
};

struct Job {
    int id;
    Prioridad prioridad;
    EstadoJob estado;
};

#endif

