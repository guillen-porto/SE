FREERTOS = free_rtos

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -I ./includes -I ./drivers -I ./${FREERTOS}/include -I ${FREERTOS}/include/private -I ${FREERTOS}/FreeRTOS/portable/GCC/ARM_CM0 \
		 -Wall -O2 -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

FREERTOS_SRC = ${FREERTOS}/FreeRTOS/list.c ${FREERTOS}/FreeRTOS/queue.c ${FREERTOS}/FreeRTOS/tasks.c \
			   ${FREERTOS}/FreeRTOS/timers.c\
			   ${FREERTOS}/FreeRTOS/portable/MemMang/heap_4.c ${FREERTOS}/FreeRTOS/portable/GCC/ARM_CM0/port.c \
               ${FREERTOS}/FreeRTOS/portable/GCC/ARM_CM0/fsl_tickless_systick.c

#Archivos que se necesitan para la compilación tanto de hello world como de led blinky
SRCS = startup.c $(wildcard drivers/*.c) $(FREERTOS_SRC) main.c
OBJS = $(SRCS:.c=.o)

	
#all: Genera los ejecutables para hello world y para led blinky
all: main.bin

#Regla general para compilar archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Crea el ejecutable main.elf
main.elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

#Flashea el fichero
flash: main.bin
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#clean: Elimina los ficheros .o (incluye explícitamente los de drivers)
clean: 
	$(RM)  $(OBJS)

#cleanall: elimina ficheros .o (incluidos los de drivers), .elf y .map 
cleanall:
	$(RM)  $(OBJS) *.elf *.bin output.map