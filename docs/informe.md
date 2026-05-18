# Informe Tecnico

## Responsabilidad del Integrante 1

La responsabilidad del Integrante 1 fue implementar la entrada de trabajos al sistema. Para esto se definio la estructura Job, los productores concurrentes y la Message Queue compartida.

## Estructura Job

Cada Job contiene:

- ID unico.
- Prioridad.
- Estado actual.

Las prioridades utilizadas son:

- FREE = 0
- PREMIUM = 1

Los estados posibles son:

- CREADO
- EN_COLA
- ASIGNADO_VRAM
- FINALIZADO

## Productores

Los productores representan nodos API Gateway. Cada productor se ejecuta en un hilo independiente y genera jobs de forma continua.

Cada job creado se inserta en la Message Queue con un retardo simulado de 100ms entre peticiones.

## Message Queue

La Message Queue fue implementada usando dos colas internas:

- Una cola para jobs Premium.
- Una cola para jobs Free.

Esta decision permite despachar primero los jobs Premium, respetando el requerimiento de prioridad.

## Sincronizacion

El acceso a la Message Queue se protege mediante mutex para evitar condiciones de carrera.

Ademas, se utiliza una condition_variable para que los consumidores esperen hasta que haya jobs disponibles, evitando busy waiting.

## Interfaz ofrecida a otros modulos

El modulo expone las siguientes operaciones:

- encolar(Job job)
- obtenerSiguiente()
- estaVacia()

Estas funciones permiten que otros integrantes puedan conectar los workers y el Pool de VRAM sin modificar la logica de produccion.