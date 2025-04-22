#include "MKL46Z4.h"
#include "lcd.h"

#define INPUT 0xABCDEF12

extern unsigned int reverse_int(unsigned int in);

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32 kHz, 1 = 4 MHz
}

void systick_ini(){
  SysTick->CTRL = 0; // Disable SysTick during setup
  SysTick->LOAD = 0xFFFFFF; // Max 24-bit value
  SysTick->VAL = 0;         // Clear current value
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

uint32_t systick_timestamp(void) {
  return SysTick->VAL;
}

//Custom delay function. Delay(3) would be equivalent to 3 normal calls to delay()
void delay()
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}


int main(void){
  SIM->COPC = 0;
  irclk_ini();
  systick_ini();
  lcd_ini();
  uint32_t start = systick_timestamp();
  reverse_int(INPUT);
  uint32_t end = systick_timestamp();
  uint32_t elapsed_ticks = start - end;
  lcd_display_dec(elapsed_ticks);
}