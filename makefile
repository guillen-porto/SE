CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
CFLAGS = -I ./includes -I ./drivers -Wall -O2 -mthumb -mcpu=cortex-m0plus
ASFLAGS = -mthumb -mcpu=cortex-m0plus
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

#Archivos que se necesitan para la compilación de todas las versiones
COMMON_SRCS = startup.c $(wildcard drivers/*.c) main.c

SRCS1 = reverse1.c $(COMMON_SRCS)
OBJS1 = $(SRCS1:.c=.o)

SRCS2 = reverse2.s $(COMMON_SRCS)
OBJS2 = $(SRCS2:.c=.o)
OBJS2 := $(OBJS2:.s=.o)

SRCS3 = reverse3.s $(COMMON_SRCS)
OBJS3 = $(SRCS3:.c=.o)
OBJS3 := $(OBJS3:.s=.o)

SRCS4 = reverse4.c $(COMMON_SRCS)
OBJS4 = $(SRCS4:.c=.o)
	
#all: Genera los ejecutables para todas las versiones
all: main1.elf main2.elf main3.elf main4.elf

#Crea el ejecutable para la versión en C inicial
main1.elf: $(OBJS1)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable para la versión en ensamblador inicial
main2.elf: $(OBJS2)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable para la versión en ensamblador optimizada
main3.elf: $(OBJS3)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable para la versión en C optimizada
main4.elf: $(OBJS4)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Regla general para compilar archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Regla general para compilar archivos en ensamblador
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@


#Flashea el programa original
flash_1: main1.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#Flashea el programa optimizado con código ensamblador aparte
flash_2: main2.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"


#Flashea el programa optimizado con código ensamblador inline
flash_3: main3.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#Flashea el programa optimizado con código ensamblador inline
flash_4: main4.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#clean: Elimina los ficheros .o (incluye explícitamente los de drivers)
clean: 
	$(RM) *.o drivers/*.o

#cleanall: elimina ficheros .o (incluidos los de drivers), .elf y .map 
cleanall:
	$(RM) *.o *drivers/*.o .elf *.map