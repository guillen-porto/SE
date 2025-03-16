#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98 a nivel global)
// LED_RED = PTE29 (pin 26 a nivel global)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73 a nivel global)
// LEFT (SW2) = PTC12 (pin 88 a nivel global)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

//Red LED
void led_red_init()
{

  SIM->SCGC5 |= SIM_SCGC5_PORTE(1); //Activate the port E

  PORTE->PCR[29] |= PORT_PCR_MUX(1);  //Activates the pin 29 of that port (corresponding to the red led)

  GPIOE->PDDR |= GPIO_PDDR_PDD(1 << 29);  //Configures the pin as input (1)

  GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29);  //Sets the value of the PDOR to 0 (clear)
}

void led_red_set()
{
  GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29);
}

void led_red_clear()
{
  GPIOE->PCOR |=GPIO_PCOR_PTCO(1 << 29);
}

//Green LED
void led_green_init(){

    SIM->SCGC5 |= SIM_SCGC5_PORTD(1); //Activate the port D
  
    PORTD->PCR[5] |= PORT_PCR_MUX(1); //Activates the pin 5 of that port (corresponding to the green led)
  
    GPIOD->PDDR |= GPIO_PDDR_PDD(1 << 5); //Configures the pin as input (1)
  
    GPIOD->PSOR |= GPIO_PSOR_PTSO(1 << 5); //Sets the value of the PDOR to 0 (clear)
}

void led_green_set()
{
    GPIOD->PSOR |= GPIO_PSOR_PTSO(1 << 5);
}

void led_green_clear()
{
    GPIOD->PCOR |=GPIO_PCOR_PTCO(1 << 5);
}

//Right switch
void right_switch_init(void){
  SIM->SCGC5 |= SIM_SCGC5_PORTC(1);

  PORTC->PCR[3] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[3] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[3] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 3)); //Hace que el pin 3 del puerto C sea de entrada (0)

}

//Left switch
void left_switch_init(void){

  PORTC->PCR[12] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[12] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 12)); //Hace que el pin 12 del puerto C sea de entrada (0)
}



//Port C and D interrupt handler:
void PORTDIntHandler(void) {
  
  if (PORTC->ISFR & (1 << 3)) { //Left switch pressed
      PORTC->ISFR |= (1 << 3);  // Clear interrupt flag

      // Action for button on PTC3 (e.g., toggle LED on PTD5)
      PTD->PTOR = (1 << 5);
  }

  // Check if PTC12 caused the interrupt
  if (PORTC->ISFR & (1 << 12)) {
      PORTC->ISFR |= (1 << 12);  // Clear interrupt flag

      // Action for button on PTC12 (e.g., toggle LED on PTE29)
      PTE->PTOR = (1 << 29);
  }
}


void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

int main(void)
{
  irclk_ini(); // Enable internal ref clk to use by LCD

  lcd_ini();
  lcd_display_dec(666);

  SIM->COPC = 0; //Disable Watchdog

  //Enable LEDs
  led_green_init();
  led_red_init();

  //Enable switches
  left_switch_init();
  right_switch_init();
  NVIC_EnableIRQ(PORTC_PORTD_IRQn); //Enable the interruption for the switches

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;

  while (index < 32) {
    if (sequence & (1 << index)) { //odd
      
      // Switch on green led
      led_green_set();
      delay();
      led_green_clear();
      //
    } else { //even
      //
      // Switch on red led
      led_red_set();
      delay();
      led_red_clear();
      // [...]
      //
    }
    // [...]
  }

  // Stop game and show blinking final result in LCD: hits:misses
  // [...]
  //

  while (1) {
  }

  return 0;
}
