CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -mthumb -mcpu=cortex-m0plus -O2
LDFLAGS = -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld
SRCS = main.c startup.c
OBJS = $(SRCS:.c=.o)

	
main.elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

flash: main.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

clean: 
	$(RM) *.o

cleanall:
	$(RM) *.o *.elf