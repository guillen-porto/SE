CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -I ./drivers -Wall -O2 -mthumb -mcpu=cortex-m0plus -D CPU_MKL46Z256VLL4
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

#Archivos que se necesitan para la compilación tanto de hello world como de led blinky
COMMON_SRCS = startup.c $(wildcard drivers/*.c) board.c clock_config.c
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

HELLO_SRCS = pin_mux.c hello_world.c
HELLO_OBJS = $(HELLO_SRCS:.c=.o)

LED_SRCS = pin_mux.c led_blinky.c
LED_OBJS = $(LED_SRCS:.c=.o)
	
#all: Genera los ejecutables para hello world y para led blinky
all: hello_world.elf led_blinky.elf

#Regla general para compilar archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Crea el ejecutable hello_world.elf
hello_world.elf: $(COMMON_OBJS) $(HELLO_OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable led_blinky.elf
led_blinky.elf: $(COMMON_OBJS) $(LED_OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Flashea el fichero hello_world
flash_hello: hello_world.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#Flashea el fichero led_blinky
flash_led: led_blinky.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#clean: Elimina los ficheros .o (incluye explícitamente los de drivers)
clean: 
	$(RM) *.o drivers/*.o

#cleanall: elimina ficheros .o (incluidos los de drivers), .elf y .map 
cleanall:
	$(RM) *.o *drivers/*.o .elf *.map