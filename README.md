# Trabajo Tutelado 1 - SE 24/25

Este proyecto consiste en implementar un temporizador programable en la placa FRDM-KL46Z mediante uno de sus temporizadores internos: el TPM (Timer/PWM Module).

## Funcionamiento del programa

El programa inicia mostrándole al usuario los valores de cuenta y alarma en el LCD, en formato ALARMA : CUENTA.

### 1 - Configuración del valor del temporizador

En este primer estado, el valor de **CUENTA** comienza a parpadear para mostrarle al usuario que es el valor que está siendo modificado. El usuario puede **aumentar** este valor  pulsando el **botón izquierdo** de la placa o **pasar al siguiente estado** pulsando el **botón derecho**.
Si el valor de cuenta llega a 99 (valor máximo), aumentarlo volverá a dejarlo en 0.

### 2 - Configuración del valor de alarma

Este estado es similar al anterior, pero ahora parpadea el valor de **ALARMA**. El usuario puede **aumentarlo** con el botón izquierdo o **pasar al siguiente estado** con el botón derecho.
Si el valor de alarma llega a 99 (valor máximo), aumentarlo volverá a dejarlo en 0.
En caso de que el usuario introduzca un valor de alarma mayor que el de cuenta, al intentar pasar a la siguiente fase se mostrará un error en el LCD.

### 3 - Modo de cuenta atrás

En este modo se realiza la cuenta atrás, reduciendo el valor de la cuenta en 1 cada segundo. Una vez el valor de cuenta sea menor o igual que el de alarma, los LEDs de la placa empezarán a parpadear periódicamente (cambiando su estado una vez por segundo).

El usuario puede **pausar o reanudar** la cuenta en cualquier momento pulsando el botón izquierdo.

### 4 - Finalización del programa

Al pasar a este estado, sucede lo siguiente:

- Se deshabilitan las interrupciones, por lo que el usuario no puede interactuar con el sistema como en los anteriores pasos
- Se apagan los LEDs en caso de que hubieran quedado encendidos al finalizar la fase anterior.
- En el LCD de la placa, el valor de la cuenta pasará a parpadear periódicamente.

## Detalles de implementación

A continuación se detallarán algunos detalles de interés sobre la implementación del programa:

### Elección de reloj

Para su funcionamiento, el TPM utiliza uno de los relojes de la placa, lo cual se debe decidir mediante el valor del registro `SIM_SOPT_TPMSRC`. Yo decidí utilizar el **IRCLK (Internal Reference Clock)**, ya que era el que tenía una frecuencia más baja (permitiendo crear un temporizador de 1Hz de frecuencia fácilmente) y porque este reloj ya debía ser activado para utilizar el LCD, por lo cual el reutilizarlo evita el activar más relojes innecesariamente.

### Obtención de una frecuencia de 1 Hz

Para obtener la frecuencia final de **1 Hz** a partir de la frecuencia del IRCLK **(32 kHz)**, utilicé tanto el **módulo** como el ***prescaler***.

- Para el valor del prescaler, utilicé el máximo posible **(128)**, ya que supuse que supondría un menor consumo que utilizar solo el módulo (ya que el prescaler reduce la frecuencia con la que se cuenta, mientras que el módulo solo aumenta el número hasta el que hay que contar para que suceda una interrupción).

- Para el valor del módulo, tuve en cuenta que, en el modo ***up-counting*** del temporizador, el contador interno del TPM incrementa cíclicamente de 0 a **MOD**, haciendo que la frecuencia final siga la fórmula: $ f_{\text{final}} = \frac{f_{\text{clk}}}{\text{prescaler} \times (\text{MOD} + 1)} $, por lo que $ MOD = \frac{f_{\text{clk}}}{f_{\text{final}} \times \text{prescaler}} - 1 $.
Sustituyendo en la anterior fórmula, se obtiene que $ MOD = \frac{32000}{{1} \times 128} - 1 = 250 - 1 = 249$.
