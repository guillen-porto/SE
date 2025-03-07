CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -I ./drivers -Wall -O2 -mthumb -mcpu=cortex-m0plus -D CPU_MKL46Z256VLL4
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

#Archivos que se necesitan para la compilación de hello world y de led blinky
COMMON_SRCS = startup.c $(wildcard drivers/*.c) board.c clock_config.c
COMMON_OBJS = $(COMMON_SRCS:.c=.o)
	

all: hello_world.elf led_blinky.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

hello_world.elf: $(COMMON_OBJS) pin_mux.o hello_world.o
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

led_blinky.elf: $(COMMON_OBJS) pin_mux2.o led_blinky.o
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

flash_hello: hello_world.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

flash_led: led_blinky.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

clean: 
	$(RM) *.o drivers/*.o

cleanall:
	$(RM) *.o *drivers/*.o .elf *.map