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


## Responsabilidad del Integrante 2
La responsabilidad del Integrante 2 fue implementar los Workers y el Pool de VRAM del sistema.

### Pool de VRAM
El Pool de VRAM representa el recurso compartido donde los jobs son procesados por los workers. Se implementó una capacidad máxima de 5 slots simultáneos, respetando el requerimiento solicitado en la consigna. 

Para controlar el acceso concurrente al Pool de VRAM se utilizó un mutex junto con una condition_variable.
- El mutex protege la variable compartida slotsOcupados, evitando que dos workers modifiquen el estado de la VRAM al mismo tiempo.
- La condition_variable permite que los workers esperen cuando la VRAM se encuentra llena, evitando busy waiting y sincronizando correctamente el acceso a los slots disponibles.

### Asignacion de Jobs a VRAM
La funcion asignarVram() se encarga de:
- Verificar si existen slots disponibles.
- Esperar cuando la VRAM alcanza el limite de capacidad.
- Incrementar la cantidad de slots ocupados.
- Cambiar el estado del job a ASIGNADO_VRAM.
- Mostrar por consola el estado de la asignacion realizada.

Tambien se agrego el retardo obligatorio de 450ms solicitado en la consigna para la asignacion de jobs al Pool de VRAM. 

### Liberacion de VRAM
La funcion liberarVram() se encarga de:
- Liberar el slot utilizado por el worker.
- Disminuir la cantidad de slots ocupados.
- Cambiar el estado del job a FINALIZADO.
- Despertar a otro worker en espera mediante notify_one().

Ademas, se implemento el retardo obligatorio de 250ms para la liberacion de slots del Pool de VRAM. 

### Workers
Los workers representan los consumidores del sistema. Cada worker se ejecuta en un hilo independiente y obtiene jobs desde la Message Queue compartida.

El flujo implementado por cada worker es:
- Obtener un job desde la cola.
- Solicitar acceso al Pool de VRAM.
- Procesar el job durante un minimo de 600ms.
- Liberar el slot utilizado.

El tiempo minimo de procesamiento tambien respeta el requerimiento solicitado en la consigna. 

### Sincronizacion
Para evitar problemas de concurrencia se utilizaron las siguientes herramientas:
- `std::mutex`
- `std::condition_variable`
- `std::unique_lock`

La sincronizacion implementada garantiza que:
- Nunca existan mas de 5 jobs activos en VRAM al mismo tiempo.
- Los workers esperen correctamente cuando no hay espacio disponible.
- No existan accesos simultaneos invalidos sobre el recurso compartido.