CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -I ./drivers -D CPU_MKL46Z256VLL4 -mthumb -mcpu=cortex-m0plus -O2
LDFLAGS = -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

COMMON_SRCS = $(wildcard drivers/*.c) board.c clock_config.c startup.c
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
	$(RM) *.o

cleanall:
	$(RM) *.o *.elf