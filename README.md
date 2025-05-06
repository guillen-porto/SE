# Práctica 5 - SE 24/25

Esta práctica consiste en implementar un sistema de productores consumidores que acceden  una cola compartida utiilzando el sistema operativo **FreeRTOS** (obtenido de la la sdk de la **FRDM-KL46Z**)

## Funcionamiento del programa

El programa tiene las siguientes características:

- Las 2 primeras cifras del LCD corresponden al número de tareas reastantes en la cola, y las dos siguientes al número de productores y consumidores, respectivamente
- Con el botón izquierdo de la placa se pueden aumentar el número de productores, y con el derecho, el de consumidores.
- El límite de productores y de consumidores es 5. Si se intenta aumentar en este punto, volverá a 0.


## Detalles de implementación

A continuación se detallarán algunos detalles de interés sobre la implementación del programa:

### Actualización del LCD

Para actualizar el LCD, se creó una tarea independiente que lo actualiza de manera periódica. Esto se hizo para que la frecuencia de actualización no dependiese del número de productores/consumidores.

### Obtención de una frecuencia de 1 Hz

Para obtener la frecuencia final de **1 Hz** a partir de la frecuencia del IRCLK **(32 kHz)**, utilicé tanto el **módulo** como el ***prescaler***.

- Para el valor del prescaler, utilicé el máximo posible **(128)**, ya que supuse que supondría un menor consumo que utilizar solo el módulo (ya que el prescaler reduce la frecuencia con la que se cuenta, mientras que el módulo solo aumenta el número hasta el que hay que contar para que suceda una interrupción).

- Para el valor del módulo, tuve en cuenta que, en el modo ***up-counting*** del temporizador, el contador interno del TPM incrementa cíclicamente de 0 a **MOD**, haciendo que la frecuencia final siga la fórmula: f_final = f_clk / (prescaler × (MOD + 1)), por lo que MOD = (f_clk / (f_final × prescaler)) - 1.
Sustituyendo en la anterior fórmula, se obtiene que MOD = (32000 / (1 × 128)) - 1 = 250 - 1 = 249
