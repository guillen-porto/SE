TARGETS = main.elf debug.elf

all: $(TARGETS)


CC = arm-none-eabi-gcc
CFLAGS = -I ./includes -mthumb -mcpu=cortex-m0plus
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld
SRCS = main.c startup.c
OBJS = $(SRCS:.c=.o)
OBJS_DBG = $(SRCS:.c=_dbg.o)


%.o: %.c
	$(CC) $(CFLAGS) -O2 -c $<

%_dbg.o: %.c
	$(CC) $(CFLAGS) -g -O0 -c -o $@ $<

main.elf: $(OBJS)

debug.elf: $(OBJS_DBG)

$(TARGETS):
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@ 


flash: main.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"


clean: 
	$(RM) *.o


cleanall:
	$(RM) *.o *.elf