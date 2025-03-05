CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -D CPU_MKL46Z256VLL4 -mthumb -mcpu=cortex-m0plus -O2
LDFLAGS = -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld
SRCS = main.c startup.c
OBJS = $(SRCS:.c=.o)
	

all: hello_world.elf led_blinky.elf

startup.o: startup.c

hello_world.o: board.c clock_config.c pin_mux.c  hello_world.c

led_blinky.o: board.c clock_config.c pin_mux2.c led_blinky.c

hello_world.elf: startup.o hello_world.o
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

led_blinky.elf: startup.o led_blinky.o
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

flash_hello: hello_world.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

flash_led: led_blinky.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

clean: 
	$(RM) *.o

cleanall:
	$(RM) *.o *.elf