# Práctica 5 - SE 24/25

Esta práctica consiste en implementar un sistema de productores consumidores que acceden  una cola compartida utilzando el sistema operativo **FreeRTOS** (obtenido de la la SDK de la **FRDM-KL46Z**)

## Funcionamiento del programa

El programa tiene las siguientes características:

- Las 2 primeras cifras del LCD corresponden al número de tareas restantes en la cola, y las dos siguientes al número de productores y consumidores, respectivamente
- Con el botón izquierdo de la placa se pueden aumentar el número de productores, y con el derecho, el de consumidores.
- El límite de productores y de consumidores es 5. Si se intenta aumentar en este punto, volverá a 0.


## Detalles de implementación

A continuación se detallarán algunos detalles de interés sobre la implementación del programa:

### Actualización del LCD

Para actualizar el LCD, se creó una tarea independiente que lo actualiza de manera periódica. Esto se hizo para que la frecuencia de actualización no dependiese del número de productores/consumidores.

### "Pool" de tareas

Para implementar la activación y desactivación de los hilos, había dos opciones principales:

- Creación de todos los hilos al iniciar el programa, y uso de una variable para comprobar si un hilo está activo (una especie de "pool" de hilos)
- Creación y destrucción dinámica de los hilos que suceda en las interrupciones.

Debido a que el número máximo de hilos de cada tipo era conocido (y bajo), decidí que la mejor opción era la primera. Para controlar si un hilo estaba activo o no, decidí que cada uno de ellos tuviese un número del 0 al 4. Si el número del hilo es menor que el número de productores actual, este estará activo. En caso contrario, estará inactivo. Esto es algo menos flexible que, por ejemplo, asignar un booleano a cada hilo que se pueda modificar desde otras partes del programa (como desde las interrupciones), pero consideré que en este caso esta versión simple era suficiente.