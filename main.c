#include "MKL46Z4.h"
#include "lcd.h"

extern unsigned int reverse_int(unsigned int in);

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32 kHz, 1 = 4 MHz
}

//Custom delay function. Delay(3) would be equivalent to 3 normal calls to delay()
void delay(int mult)
{
  volatile int i;
  volatile int waitTime = 1000000 * mult;

  for (i = 0; i < waitTime; i++);
}


int main(void){

    irclk_ini();
    lcd_ini();

    unsigned int original = 0xA690;
    unsigned int reversed;
    unsigned int displayed;

    reversed = reverse_int(original);
    displayed = reversed >> 16; //First 4 hex digits of reversed (first 16 bits of the 32)

    lcd_display_hex(original);
    delay(2); //Double delay
    lcd_display_hex(displayed);
}