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
  GPIOD->PDDR |= GPIO_PDDR_PDD(1 << 5);

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

  GPIOE->PDDR |= GPIO_PDDR_PDD(1 << 29);

  GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29);
}

void led_red_toggle(void)
{
  //
  GPIOE->PTOR |= GPIO_PTOR_PTTO(1 << 29);
}

int main(void)
{
  led_green_init();
  led_red_init();

  while (1) {
    led_green_toggle();
    //Inicialmente delay aquí
    delay();
    led_red_toggle();
    //delay();
  }

  return 0;
}
