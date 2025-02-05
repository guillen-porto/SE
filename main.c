#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init()
{
  // SIM->COPC
  SIM->COPC |= SIM_COPC_COPT(0); //Se puede poner directamente SIM->COPC |= 0

  // SIM->SCGC5
  //SIM->SCGC5 |= SIM_SCGC5_PORTD(1);
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);

  // PORTD->PCR[5]
  PORTD->PCR[5] |= PORT_PCR_MUX(1);

  // GPIOD->PDDR
  GPIOD->PDDR |= GPIO_PDDR_PDD(1 << 5); //Hace que el pin 5 del puerto D sea de salida (1)

  // GPIOD->PSOR
  GPIOD->PSOR |= GPIO_PSOR_PTSO(1 << 5);
}

void led_green_toggle()
{
  //
    GPIOD->PTOR |= GPIO_PTOR_PTTO(1 << 5);
}

// LED_RED = PTE29
void led_red_init()
{

  //SIM->SCGC5 |= SIM_SCGC5_PORTE(1);
  SIM->SCGC5 |= SIM_SCGC5_PORTE(1);

  PORTE->PCR[29] |= PORT_PCR_MUX(1);

  GPIOE->PDDR |= GPIO_PDDR_PDD(1 << 29); //Hace que el pin 29 del puerto E sea de salida (1)

  GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29);
}

void led_red_toggle(void)
{
  //
  GPIOE->PTOR |= GPIO_PTOR_PTTO(1 << 29);
}

//SWITCH1= PTC3
void switch1_init(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTC(1);

    PORTC->PCR[3] |= PORT_PCR_MUX(1);

    PORTC->PCR[3] |= PORT_PCR_PE(1); //Pull enable

    PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

    GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 3)); //Hace que el pin 3 del puerto C sea de entrada (0)

}

int switch1_pressed(void){
    return (GPIOC->PDIR & (1 << 3)) == 0;
}

//SWITCH3= PTC12
void switch3_init(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //entiendo que solo hace falta inicializarlo una vez (en button_green_init)

    PORTC->PCR[12] |= PORT_PCR_MUX(1);

    PORTC->PCR[12] |= PORT_PCR_PE(1); //Pull enable

    PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

    GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 12)); //Hace que el pin 12 del puerto C sea de entrada (0)
}

int switch3_pressed(void){
    return (GPIOC->PDIR & (1 << 12)) == 0;
}


int main(void)
{
  led_green_init();
  led_red_init();

  switch1_init();
  switch3_init();

  while (1) {

    if(switch1_pressed()){
      led_green_toggle();
      delay();
    }
    //Inicialmente delay aquí

    if(switch3_pressed()){
      led_red_toggle();
      delay();
    }
    //delay();
  }

  return 0;
}
