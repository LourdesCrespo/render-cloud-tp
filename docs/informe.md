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

## Responsabilidad del Integrante 3

La responsabilidad del Integrante 3 fue implementar los mecanismos de sincronizacion del sistema, proteger el contador global de jobs finalizados y diseñar la estrategia anti-starvation para evitar que los jobs Free queden esperando indefinidamente.

## Contador Global de Jobs Finalizados

El sistema utiliza un contador global para registrar la cantidad de jobs procesados.

Para proteger este contador se utilizo una variable atomica:

`std::atomic<int> jobsFinalizados(0);`

De esta forma varios Workers pueden actualizar el valor al mismo tiempo sin generar errores de concurrencia.

Tambien se utilizo la variable:

`std::atomic<bool> sistemaActivo(true);`

para controlar el apagado ordenado de los Workers al finalizar las pruebas.

## Sincronizacion

Para coordinar la ejecucion concurrente del sistema se utilizaron distintas herramientas de sincronizacion.

### Mutex

Se utilizaron mutex para proteger recursos compartidos y evitar accesos simultaneos incorrectos.

Los mutex se encuentran en:

* Message Queue.
* Pool de VRAM.
* Logger.
* Generacion de IDs.

Gracias a esto se evita que varios hilos modifiquen la misma informacion al mismo tiempo.

### Condition Variable

La Message Queue utiliza una `condition_variable` para que los Workers esperen cuando no existen jobs disponibles.

Cuando un Productor agrega un nuevo Job a la cola se despierta a un Worker mediante:

`hayJobs.notify_one();`

Y al finalizar el sistema se utiliza:

`hayJobs.notify_all();`

para despertar a todos los Workers que puedan estar esperando.

### Semaforo

El sistema utiliza un semaforo para controlar el acceso al Pool de VRAM.

`Semaforo semaforoSlots(5);`

Esto permite limitar la cantidad de jobs que pueden utilizar la VRAM al mismo tiempo.

## Prevencion de Starvation

Para evitar que los Jobs Free queden bloqueados por una llegada constante de Jobs Premium, se implemento una estrategia anti-starvation.

El sistema registra cuando comienza un flujo continuo de Jobs Premium.

Si pasan mas de 5000 ms procesando solamente Jobs Premium y existen Jobs Free esperando, se selecciona un Job Free para su ejecucion.

De esta forma se garantiza que todos los Jobs tengan oportunidad de ser procesados.

## Analisis de Deadlock

Se analizo la posibilidad de Deadlock y no se encontraron situaciones de bloqueo permanente.

Esto se debe a que los recursos se liberan correctamente y no existen esperas circulares entre hilos.

## Analisis de Livelock

Tampoco se detectaron situaciones de Livelock.

Los Workers siempre avanzan en el procesamiento de los Jobs y no existen ciclos de reintentos que impidan el progreso del sistema.

## Justificacion de las Primitivas Utilizadas

Las primitivas utilizadas fueron:

* `std::mutex`
* `std::condition_variable`
* `std::atomic`
* Semaforo

Estas herramientas permitieron proteger los recursos compartidos, coordinar la ejecucion de los hilos y garantizar el correcto funcionamiento del sistema concurrente.

### **4\. Protocolo de Ciclo de Vida y Apagado Seguro** 

### Una de las mayores complejidades añadidas a la arquitectura concurrente del proyecto radica en el control del final de la ejecución. En lugar de forzar la terminación abrupta del proceso mediante el abandono de la función `main()` o el uso de llamadas riesgosas como `.detach()` (las cuales inducen a fugas de recursos, destrucción de objetos compartidos en pleno uso y corrupción de memoria en tiempo de ejecución), se diseñó un protocolo de Apagado Seguro coordinado en dos fases:

1. **Flag de Control Atómico Externo:** Se introdujo la variable global `std::atomic<bool> sistemaActivo(true)` expuesta a través del header `Worker.h`. Al ser de tipo atómico, se garantiza que las operaciones de lectura realizadas por los múltiples hilos consumidores dentro de sus bucles de procesamiento estén completamente libres de condiciones de carrera y no requieran un mutex adicional para ser evaluadas de forma segura.  
2. **Sincronización de Salida via Barrier-Join:** Una vez que los hilos productores finalizan su carga de tareas y la cola compartida es vaciada por completo, el hilo orquestador de pruebas conmuta la bandera a `false` y ejecuta un bucle de recolección llamando imperativamente al método `.join()` de cada consumidor. Esto asegura que cada hilo complete de manera limpia su unidad de trabajo actual, libere adecuadamente los recursos compartidos de la VRAM y finalice de forma ordenada registrando su evento correspondiente en el archivo de auditoría.
