CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
CFLAGS = -I ./includes -I ./drivers -Wall -O2 -mthumb -mcpu=cortex-m0plus
ASFLAGS = -mthumb -mcpu=cortex-m0plus
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld

#Archivos que se necesitan para la compilación de todas las versiones
COMMON_SRCS = startup.c $(wildcard drivers/*.c) main.c

ORIG_SRCS = reverse_int_orig.s $(COMMON_SRCS)
ORIG_OBJS = $(ORIG_SRCS:.c=.o)
ORIG_OBJS := $(ORIG_OBJS:.s=.o) #Añade los objetos creados a partir de ficheros .s

OPT_SRCS = reverse_int_opt.s $(COMMON_SRCS)
OPT_OBJS = $(OPT_SRCS:.c=.o)
OPT_OBJS := $(OPT_OBJS:.s=.o)

OPT_INL_SRCS = reverse_int_opt_inl.c $(COMMON_SRCS)
OPT_INL_OBJS = $(OPT_INL_SRCS:.c=.o)
	
#all: Genera los ejecutables para todas las versiones
all: main_orig.elf main_opt.elf main_opt_inl.elf

#Crea el ejecutable para la versión sin optimizar
main_orig.elf: $(ORIG_OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable para la versión optimizada con código ensamblador aparte
main_opt.elf: $(OPT_OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Crea el ejecutable para la versión optimizada con código ensamblador inline
main_opt_inl.elf: $(OPT_INL_OBJS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

#Regla general para compilar archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Regla general para compilar archivos en ensamblador
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@


#Flashea el programa original
flash_orig: main_orig.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#Flashea el programa optimizado con código ensamblador aparte
flash_opt: main_opt.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"


#Flashea el programa optimizado con código ensamblador inline
flash_opt_inl: main_opt_inl.elf
	openocd -f openocd.cfg -c "program $^ verify reset exit"

#clean: Elimina los ficheros .o (incluye explícitamente los de drivers)
clean: 
	$(RM) *.o drivers/*.o

#cleanall: elimina ficheros .o (incluidos los de drivers), .elf y .map 
cleanall:
	$(RM) *.o *drivers/*.o .elf *.map