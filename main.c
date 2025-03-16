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

volatile int button_pressed = 0; //Variable that indicates that a button has been pressed (1)

int currentState = 0; //State of the led, 0 means red is turned on and 1 means green is turned on

int hits = 0; //Number of correct presses
int misses = 0; //Number of wrong presses


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
  SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //Enable port C4


  PORTC->PCR[3] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[3] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[3] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 3)); //Sets pin 3 of port c as input(0)

  PORTC->PCR[3] |= PORT_PCR_IRQC(10); //10 (1010): Interrupt on falling edge (when buttoon is pressed) 
}

//Left switch
void left_switch_init(void){

  SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //Enable port C4 (probably not needed)

  PORTC->PCR[12] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[12] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 12)); //Sets pin 12 of port c as input(0)

  PORTC->PCR[3] |= PORT_PCR_IRQC(10); //10 (1010): Interrupt on falling edge (when buttoon is pressed) 
}



//Port C and D interrupt handler:
void PORTDIntHandler(void) {
  
  if (PORTC->ISFR & (1 << 3)) { //Right switch pressed
      PORTC->ISFR |= (1 << 3);  // Clear interrupt flag

      if(currentState == 1){ //Correct if led is currently green
        hits ++;
      }
      else{
        misses ++;
      }
      button_pressed = 1;
      
  }

  if (PORTC->ISFR & (1 << 12)) { //Left switch presssed
      PORTC->ISFR |= (1 << 12);  // Clear interrupt flag
   
      if(currentState == 0){ //Correct if led is currently red
        hits ++;
      }
      else{
        misses ++;
      }

      button_pressed = 1;
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
  lcd_display_time(0, 0);

  SIM->COPC = 0; //Disable Watchdog

  //Enable LEDs
  led_green_init();
  led_red_init();

  //Enable switches
  right_switch_init();
  left_switch_init();

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;

  NVIC_EnableIRQ(PORTC_PORTD_IRQn); //Game starts, enable interruptions for the switches

  while (index < 32) {
    button_pressed = 0; //Reset the button_pressed variable
    if (sequence & (1 << index)) { //odd
      
      // Switch on green led
      currentState = 1;
      led_green_set();

      while(!button_pressed); //Wait until a button is pressed to clear the led and go to the next iteration

      led_green_clear();

    } else { //even

      // Switch on red led
      currentState = 0;
      led_red_set();
      while(!button_pressed); //Wait until a button is pressed to clear the led and go to the next iteration
      led_red_clear();

    }

    lcd_display_time(hits, misses); //Displays hist:misses
    index ++; //Go to next number in sequence
  }
  NVIC_DisableIRQ(PORTC_PORTD_IRQn); //Game ended, disable interruptions for the switches
  // [...]
  //
  while (1) {
    delay();
    lcd_display_time(hits, misses);
  }

  return 0;
}
