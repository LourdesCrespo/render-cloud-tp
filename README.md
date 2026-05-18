# render-cloud-tp
# Sistema de Renderizado en la Nube

Trabajo Practico Integrador de Programacion Concurrente.

## Integrantes

- Integrante 1: Productores, Message Queue y repositorio GitHub
- Integrante 2: Workers y Pool de VRAM
- Integrante 3: Sincronizacion, contador global y anti-starvation
- Integrante 4: Logging, pruebas y video

## Descripcion

El sistema simula una granja de renderizado en la nube.

Los productores generan jobs de renderizado y los insertan en una Message Queue compartida. Los jobs pueden tener prioridad Premium o Free.

## Compilacion

```bash
g++ src/*.cpp -o render_cloud -pthread