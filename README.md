# Práctica 4 - SE 24/25

## Estructura del proyecto

El código original (en C) se encuentra en el fichero ***reverse_int_orig.c***. El código transformado a ensamblador (mediante la instrucción `arm-none-eabi-gcc -mthumb -mcpu=cortex-m0plus -S -Ofast reverse_int_orig.c`) está en el fichero ***reverse_int_orig.s***.
Las versones optimizadas se encuentran en los ficheros ***reverse_int_opt.s** y **reverse_int_opt_inl.c**.

## Compilación y flasheo

La instrucción ***make all*** crea los ejecutables de las 3 versiones:
- `main_orig.elf`: Versión sin optimizar.

- `main_opt.elf`: Versión optimizada con ensamblador aparte.

- `main_opt_inl.elf`: Versión optimizada con ensamblador inline.

Si se quiere flashear una de las versiones en concreto, sería necesario usar los siguientes comandos:

- Versión original:
  ```
  make flash_orig
  ```
- Versión optimizada con ensamblador aparte:
  ```
  make flash_opt
  ```
- Versión optimizada con ensamblador inline:
  ```
  make flash_opt_inl
  ```



## Explicación de las optimizaciones

La función reverse_int original (obtenida con la versión 12.2 del compilador arm-none-eabi-gcc) es la siguiente:

```assembly
reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0
	push	{r4, lr}
	movs	r2, #32
	movs	r0, #0
	movs	r4, #1
.L2:
	movs	r1, r4
	lsls	r0, r0, #1
	ands	r1, r3
	subs	r2, r2, #1
	orrs	r0, r1
	lsrs	r3, r3, #1
	cmp	r2, #0
	bne	.L2
	@ sp needed
	pop	{r4, pc}
```

  En esta función, detecté 2 aspectos que podrían ser optimizados:

1. Al preparar los parámetros de la función, se ejecuta la instrucción `movs r4, #1`. Este registro r4 solo se utiliza una vez por iteración del bucle para hacer que el valor de r1 sea 1 (para luego hacer un and y de esta forma obtener el último bit del valor contenido en r3). Esto me pareció innecesario, ya que se podría directamente cargar la constante 1 en r1 en cada iteración. Debido a esto, eliminé esa línea y modifiqué la línea `movs r1, r4` por `movs r1, #1`. De esta forma, se utiilzan una instrucción y un registro menos.
Se me hizo extraño que el compilador lo tradujese de esta forma, ya que parece algo relativamente simple de ver, pero por lo que comprobé, modificarlo no tuvo ningún efecto secundario.
2. Hay ciertas instrucciones que se ejecutan una iteración más de la que deberían. Por ejemplo, `lsls r0, r0 #1` se ejecuta al principio de la primera iteración del bucle aunque ahí el contenido de r0 sea 0 y 	`lsrs r3, r3, #1` se ejecuta al final de la última iteración del bucle aunque en realidad solo hagan falta 31 desplazamientos para tomar todos sus bits (ya que el bit 0 se toma sin ningún desplazamiento). 
Para optimizar esto, reduje el número de iteraciones del bucle en 1, hice que el desplazamiento en r0 se ejecutase después del resto de instrucciones de la iteración, y, después de la última iteración, añadí las siguientes instrucciones, que son las que sí que es necesario ejecutar una vez más (ya que son las que toman el valor del bit menos significativo de r3 y lo ponen en r0):

```assembly
  movs	r1, #1
  	ands	r1, r3
  	orrs	r0, r1
```

Con estas dos optimizaciones, la función acabó teniendo este aspecto en mi código ensamblador:

```assembly
reverse_int:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0
	push	{lr}
	movs	r2, #31
	movs	r0, #0
.L2:
	movs	r1, #1
	ands	r1, r3
	subs	r2, r2, #1
	orrs	r0, r1
	lsls	r0, r0, #1
	lsrs	r3, r3, #1
	cmp	r2, #0
	bne	.L2
	movs	r1, #1
	ands	r1, r3
	orrs	r0, r1
	@ sp needed
	pop	{pc}
```
