main.elf:
	arm-none-eabi-gcc -I ./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus -c -o startup.o startup.c
	
	arm-none-eabi-gcc -I ./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus -c -o main.o main.c

	arm-none-eabi-gcc -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld main.o startup.o -o main.elf

compile_debug:
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o startup.o startup.c
	
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o main.o main.c

	arm-none-eabi-gcc -O0 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=main.map -T link.ld main.o startup.o -o main.elf


flash: main.elf
	openocd -f openocd.cfg -c "program main.elf verify reset exit"


clean: 
	$(RM) *.o


mrproper:
	$(RM) *.o main.elf