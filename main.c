#include "MKL46Z4.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define MAX_PRODUCERS 5
#define MAX_CONSUMERS 5


uint8_t numData = 0; //Amount of data left in the queue
uint8_t numProducers = 0; //Number of producers
uint8_t numConsumers = 0; //Number of consumers


//Initialize irlclk for the lcd
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}


//Right switch
void right_switch_init(void){
  SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //Enable port C4

  PORTC->PCR[3] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[3] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[3] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 3)); //Sets pin 3 of port c as input(0)

  PORTC->PCR[3] |= PORT_PCR_IRQC(10); //10 (1010): Interrupt on falling edge (when button is pressed) 
}

//Left switch
void left_switch_init(void){

  SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //Enable port C4 (probably not needed as it's done on the other function)

  PORTC->PCR[12] |= PORT_PCR_MUX(1); //Set as GPIO

  PORTC->PCR[12] |= PORT_PCR_PE(1); //Pull enable

  PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

  GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 12)); //Sets pin 12 of port c as input(0)

  PORTC->PCR[12] |= PORT_PCR_IRQC(10); //10 (1010): Interrupt on falling edge (when buttoon is pressed) 
}



//Port C and D interrupt handler:
void PORTDIntHandler(void) {
  
  if (PORTC->ISFR & (1 << 3)) { //Right switch pressed -> Increase consumers
      PORTC->ISFR |= (1 << 3);  // Clear interrupt flag
      if(numConsumers < 5){
        numConsumers ++;
      }
      else{
        numConsumers = 0;
      }

  }

  if (PORTC->ISFR & (1 << 12)) { //Left switch presssed -> Increase starting time/alarm time
      PORTC->ISFR |= (1 << 12);  // Clear interrupt flag
    
      if(numProducers < 5){
        numProducers ++;
      }
      else{
        numProducers = 0;
      }
  }
  uint8_t displayed = 10 * numProducers + numConsumers;
  lcd_display_time(numData, displayed);
}

//Function that the producers will execute
void addData(){

}

//Function that the consumers will execute
void removeData(){

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
  lcd_display_time(0, 0); //Display alarm and count as a time (alarm:count)

  SIM->COPC = 0; //Disable Watchdog

  //Enable switches
  right_switch_init();
  left_switch_init();

  NVIC_EnableIRQ(PORTC_PORTD_IRQn); //Enable button interruptions

  //Main loop of the program
  while(1){
    __WFI;
  }


  return 0;
}