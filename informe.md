\# Informe Tecnico

\#\# Responsabilidad del Integrante 1

La responsabilidad del Integrante 1 fue implementar la entrada de trabajos al sistema. Para esto se definio la estructura Job, los productores concurrentes y la Message Queue compartida.

\#\# Estructura Job

Cada Job contiene:

\- ID unico.  
\- Prioridad.  
\- Estado actual.

Las prioridades utilizadas son:

\- FREE \= 0  
\- PREMIUM \= 1

Los estados posibles son:

\- CREADO  
\- EN\_COLA  
\- ASIGNADO\_VRAM  
\- FINALIZADO

\#\# Productores

Los productores representan nodos API Gateway. Cada productor se ejecuta en un hilo independiente y genera jobs de forma continua.

Cada job creado se inserta en la Message Queue con un retardo simulado de 100ms entre peticiones.

\#\# Message Queue

La Message Queue fue implementada usando dos colas internas:

\- Una cola para jobs Premium.  
\- Una cola para jobs Free.

Esta decision permite despachar primero los jobs Premium, respetando el requerimiento de prioridad.

\#\# Sincronizacion

El acceso a la Message Queue se protege mediante mutex para evitar condiciones de carrera.

Ademas, se utiliza una condition\_variable para que los consumidores esperen hasta que haya jobs disponibles, evitando busy waiting.

\#\# Interfaz ofrecida a otros modulos

El modulo expone las siguientes operaciones:

\- encolar(Job job)  
\- obtenerSiguiente()  
\- estaVacia()

Estas funciones permiten que otros integrantes puedan conectar los workers y el Pool de VRAM sin modificar la logica de produccion.

\#\# Responsabilidad del Integrante 2  
La responsabilidad del Integrante 2 fue implementar los Workers y el Pool de VRAM del sistema.

\#\#\# Pool de VRAM  
El Pool de VRAM representa el recurso compartido donde los jobs son procesados por los workers. Se implementó una capacidad máxima de 5 slots simultáneos, respetando el requerimiento solicitado en la consigna. 

Para controlar el acceso concurrente al Pool de VRAM se utilizó un mutex junto con una condition\_variable.  
\- El mutex protege la variable compartida slotsOcupados, evitando que dos workers modifiquen el estado de la VRAM al mismo tiempo.  
\- La condition\_variable permite que los workers esperen cuando la VRAM se encuentra llena, evitando busy waiting y sincronizando correctamente el acceso a los slots disponibles.

\#\#\# Asignacion de Jobs a VRAM  
La funcion asignarVram() se encarga de:  
\- Verificar si existen slots disponibles.  
\- Esperar cuando la VRAM alcanza el limite de capacidad.  
\- Incrementar la cantidad de slots ocupados.  
\- Cambiar el estado del job a ASIGNADO\_VRAM.  
\- Mostrar por consola el estado de la asignacion realizada.

Tambien se agrego el retardo obligatorio de 450ms solicitado en la consigna para la asignacion de jobs al Pool de VRAM. 

\#\#\# Liberacion de VRAM  
La funcion liberarVram() se encarga de:  
\- Liberar el slot utilizado por el worker.  
\- Disminuir la cantidad de slots ocupados.  
\- Cambiar el estado del job a FINALIZADO.  
\- Despertar a otro worker en espera mediante notify\_one().

Ademas, se implemento el retardo obligatorio de 250ms para la liberacion de slots del Pool de VRAM. 

\#\#\# Workers  
Los workers representan los consumidores del sistema. Cada worker se ejecuta en un hilo independiente y obtiene jobs desde la Message Queue compartida.

El flujo implementado por cada worker es:  
\- Obtener un job desde la cola.  
\- Solicitar acceso al Pool de VRAM.  
\- Procesar el job durante un minimo de 600ms.  
\- Liberar el slot utilizado.

El tiempo minimo de procesamiento tambien respeta el requerimiento solicitado en la consigna. 

\#\#\# Sincronizacion  
Para evitar problemas de concurrencia se utilizaron las siguientes herramientas:  
\- \`std::mutex\`  
\- \`std::condition\_variable\`  
\- \`std::unique\_lock\`

La sincronizacion implementada garantiza que:  
\- Nunca existan mas de 5 jobs activos en VRAM al mismo tiempo.  
\- Los workers esperen correctamente cuando no hay espacio disponible.  
\- No existan accesos simultaneos invalidos sobre el recurso compartido.

### Responsabilidad del integrante 4

**Evasión de Deadlocks**

Para cumplir de forma estricta con las especificaciones del trabajo, se implementó un mecanismo de persistencia atómica en el archivo físico `sistema.log`. En un entorno de alta concurrencia, la escritura simultánea desde múltiples hilos productores e hilos consumidores (Workers) representaba una **Condición de Carrera**  crítica sobre el buffer del sistema operativo, capaz de corromper los datos o entorpecer el orden cronológico del registro.

Para resolverlo, se protegió la región crítica mediante un objeto de exclusión mutua global (`std::mutex logMutex`). En lugar de gestionar manualmente los bloqueos con `.lock()` y `.unlock()`.

* Si el programa llega a fallar de golpe por un error al escribir el archivo, o si la función termina antes de tiempo por un return inesperado, usar `std::lock_guard` nos salva. C++ se encarga de destruir automáticamente este bloqueo de la memoria y libera el mutex solo. Gracias a esto, nos aseguramos de que el archivo nunca quede "trabado" para siempre, evitando un **Deadlock**.

### **2\. Diseño del Framework de Pruebas Automatizado**

El archivo principal `Main.cpp` fue completamente reestructurado para funcionar como un orquestador automatizado de escenarios de evaluación. Se implementó una función modular paramétrica encargada de instanciar dinámicamente el ecosistema completo según los requisitos evaluativos del trabajo práctico:

`void ejecutarConfiguracionPrueba(int numProductores, int numConsumidores, int idEscenario, int totalJobs);`

Dicha estructura permite la instancia dinámica de referencias hacia los recursos compartidos (MessageQueue y PoolVRAM) vinculando los módulos de los diversos integrantes. Con este enfoque, es posible evaluar la robustez de las primitivas de sincronización bajo las tres topologías de hilos requeridas por la consigna:

Escenario A: un nodo productor trabajando con dos consumidores.

Escenario B: tres nodos productores y un único consumidor.

Escenario C: ejecución concurrente de tres productores y tres consumidores.  
**3\. Matriz de Validación de Escenarios Obligatorios**

La infraestructura diseñada ejecuta secuencialmente los cuatro escenarios solicitados para validar las propiedades de vivacidad del sistema bajo condiciones críticas:

| Escenario de Evaluación | Configuración de Hilos | Comportamiento Técnico Esperado |
| :---- | :---- | :---- |
| **Carga Masiva**  *(1500 Tareas)* | **Configuración A**  *(1 Prod / 2 Cons)* | Somete a estrés el contador global de tareas finalizadas con éxito. Se verifica que tras el vaciado completo, el valor final coincide deterministicamente con el número inyectado, confirmando la ausencia de condiciones de carrera mediante el uso de tipos atómicos o exclusión mutua. |
| **Vacuidad**  *(0 Tareas)* | **Configuración A**  *(1 Prod / 2 Cons)* | Los Hilos Workers ingresan en un estado de **espera pasiva** al detectar que la cola de prioridades se encuentra vacía. Al suspenderse en el kernel mediante variables de condición (std::condition\_variable), el consumo de ciclos de CPU cae al 0%, demostrando la erradicación absoluta de bucles de Espera Activa. |
| **Saturación de Recursos**  *(8 Tareas Premium)* | **Configuración B**  *(3 Prod / 1 Cons)* | Se evalúa el comportamiento límite de la capacidad física de la GPU simulada. Aunque las peticiones concurrentes de alta prioridad desbordan al sistema de forma inmediata, los hilos se bloquean de manera ordenada, garantizando que el PoolVRAM mantenga un límite estricto de **máximo 5 slots activos** en simultáneo. |
| **Equidad / Anti-Starvation**  *(Flujo asimétrico)* | **Configuración C**  *(3 Prod / 3 Cons)* | Comprueba la efectividad de la política de despacho del sistema. Tras una ventana temporal de 5000ms de inyección exclusiva de tareas de máxima prioridad, el algoritmo alternativo permite que las tareas con prioridad FREE bloqueadas en la cola recuperen la vivacidad y finalicen su ejecución con éxito. |

### **4\. Protocolo de Ciclo de Vida y Apagado Seguro** 

### Una de las mayores complejidades añadidas a la arquitectura concurrente del proyecto radica en el control del final de la ejecución. En lugar de forzar la terminación abrupta del proceso mediante el abandono de la función `main()` o el uso de llamadas riesgosas como `.detach()` (las cuales inducen a fugas de recursos, destrucción de objetos compartidos en pleno uso y corrupción de memoria en tiempo de ejecución), se diseñó un protocolo de Apagado Seguro coordinado en dos fases:

1. **Flag de Control Atómico Externo:** Se introdujo la variable global `std::atomic<bool> sistemaActivo(true)` expuesta a través del header `Worker.h`. Al ser de tipo atómico, se garantiza que las operaciones de lectura realizadas por los múltiples hilos consumidores dentro de sus bucles de procesamiento estén completamente libres de condiciones de carrera y no requieran un mutex adicional para ser evaluadas de forma segura.  
2. **Sincronización de Salida via Barrier-Join:** Una vez que los hilos productores finalizan su carga de tareas y la cola compartida es vaciada por completo, el hilo orquestador de pruebas conmuta la bandera a `false` y ejecuta un bucle de recolección llamando imperativamente al método `.join()` de cada consumidor. Esto asegura que cada hilo complete de manera limpia su unidad de trabajo actual, libere adecuadamente los recursos compartidos de la VRAM y finalice de forma ordenada registrando su evento correspondiente en el archivo de auditoría.

