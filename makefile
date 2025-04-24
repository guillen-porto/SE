CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -I ./drivers -I ./free_rtos/include -Wall -O2 -mthumb -mcpu=cortex-m0plus -D CPU_MKL46Z256VLL4
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

#Archivos que se necesitan para la compilación tanto de hello world como de led blinky
SRCS = startup.c $(wildcard drivers/*.c) board.c clock_config.c
OBJS = $(SRCS:.c=.o)
	
#all: Genera los ejecutables para hello world y para led blinky
all: main.elf

#Regla general para compilar archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Crea el ejecutable main.elf
main.elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Flashea el fichero
flash: main.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#clean: Elimina los ficheros .o (incluye explícitamente los de drivers)
clean: 
	$(RM) *.o drivers/*.o

#cleanall: elimina ficheros .o (incluidos los de drivers), .elf y .map 
cleanall:
	$(RM) *.o *drivers/*.o *.elf *.map