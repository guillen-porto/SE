# Trabajo Tutelado 2 - SE 24/25

Este trabajo consistía en implementar un programa que, utilizando el PWM, modificase la intensidad de los LEDs de la placa dependiendo de los valores obtenidos por uno de sus sensores. En mi caso, utilicé el módulo TSI (Touch Sensing Input)

## Funcionamiento del programa

El programa funciona de la siguiente manera:

- Si el usuario no está tocando el *slider* de la placa, ambos LEDs están apagados.
- Cuanto más cerca de la **izquierda** del *slider* presione el usuario, más intensamente brillará el LED **rojo**.
- Cuanto más cerca de la **derecha** del *slider* presione el usuario, más intensamente brillará el LED **verde**.

## Detalles de implementación

A continuación se detallarán algunos detalles de interés sobre la implementación del programa:

### Función de inicialización del TSI

Los valores de la función de inicialización del TSI fueron ligeramente modificados con respecto al ejemplo dado en las *slides* (<https://forum.digikey.com/t/using-the-capacitive-touch-sensor-on-the-frdm-kl46z/13246>). Esto se hizo para aumentar la precisión de medida (ya que se aumentan el número de mediciones) a costa de aumentar algo el consumo de la placa.

### Detección de la posición en el slider

Para calcular en qué zona del slider se está presionando, me ayudé de las funciones dadas en el foro antes mencionado. Creé un método propio a partir de *Touch_Scan_LH* y *Touch_Scan_HL* que me permitiese pasarle el canal por parámetro, para evitar repetición de código. Además, por seguridad, añadí la siguiente línea:
```while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK))```
Esta hace que la función espere hasta que el escaneo acabe antes de comprobar el valor del registro que contiene los datos obtenidos.

### Función para obtener los valores del LED

Este es un detalle menos importante, pero lo consideré de interés igualmente. Creé una función para obtener los valores del LED ya que, en caso de que se quisiese extender el proyecto para usar cualquier otro tipo de sensor, la firma de la función podría mantenerse constante.
