#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

int state = 0; //Realmente solo se necesitarían dos bits para el estado (los dos últimos de esta variable).
/*
00 -> P1 y P2 cerradas
01 -> P1 cerrada, P2 abierta
10 -> P1 abierta, P2 cerrada
11 -> P1 y P2 abiertas
*/

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void led_green_init()
{
  // SIM->COPC
  SIM->COPC = 0;

  // SIM->SCGC5
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);

  // PORTD->PCR[5]
  PORTD->PCR[5] |= PORT_PCR_MUX(1);

  // GPIOD->PDDR
  GPIOD->PDDR |= GPIO_PDDR_PDD(1 << 5); //Hace que el pin 5 del puerto D sea de salida (1)

  // GPIOD->PSOR
  GPIOD->PCOR |= GPIO_PSOR_PTSO(1 << 5); //PTSO setea el registro PDOR a 0 -> Enciende el led verde
}

void led_green_toggle()
{
  //
    GPIOD->PTOR |= GPIO_PTOR_PTTO(1 << 5); //PTOR hace toggle al valor del registro PTSO (1 -> 0 y 0 -> 1)
}

// LED_RED = PTE29
void led_red_init()
{

  SIM->SCGC5 |= SIM_SCGC5_PORTE(1);

  PORTE->PCR[29] |= PORT_PCR_MUX(1);

  GPIOE->PDDR |= GPIO_PDDR_PDD(1 << 29); //Hace que el pin 29 del puerto E sea de salida (1)

  GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29); //Apaga el led rojo
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

    PORTC->PCR[3] |= PORT_PCR_PS(1); //Pull up

    GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 3)); //Hace que el pin 3 del puerto C sea de entrada (0)

}

//SWITCH3= PTC12
void switch2_init(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTC(1); //entiendo que solo hace falta inicializarlo una vez (en switch1_init)

    PORTC->PCR[12] |= PORT_PCR_MUX(1);

    PORTC->PCR[12] |= PORT_PCR_PE(1); //Pull enable

    PORTC->PCR[12] |= PORT_PCR_PS(1); //Pull up

    GPIOC->PDDR &= GPIO_PDDR_PDD(~(1 << 12)); //Hace que el pin 12 del puerto C sea de entrada (0)
}


int switch1_pressed(void){
  return (GPIOC->PDIR & (1 << 3)) == 0;
}


int switch2_pressed(void){
  return (GPIOC->PDIR & (1 << 12)) == 0;
}


//Funciones de modificación de estado

//Un xor con un bit a 1 y el resto a 0 modifica el valor del bit que coincide con el 1 y deja el resto iguales
void modify_door1_state(void){
  state ^= (uint32_t)(1 << 1); //Se modifica el valor del penúltimo bit
}

void modify_door2_state(void){
  state ^= (uint32_t)1;  //Se modifica el valor del último bit
}

void display_state(void){
  if(state == 0){ //Estado seguro, encender verde y apagar rojo
    GPIOD->PCOR |= GPIO_PSOR_PTSO(1 << 5);
    GPIOE->PSOR |= GPIO_PSOR_PTSO(1 << 29); 
  }
  else{ //Estado inseguro, encender rojo y apagar verde
    GPIOD->PSOR |= GPIO_PSOR_PTSO(1 << 5); 
    GPIOE->PCOR |= GPIO_PSOR_PTSO(1 << 29); 
  }
}



int main(void)
{
  led_green_init();
  led_red_init();

  switch1_init();
  switch2_init();

  while (1) {

    if(switch2_pressed()){
      modify_door2_state();
      display_state();
      while(switch2_pressed()); //Espera a que se deje de pulsar el botón para continuar
    }

    if(switch1_pressed()){
      modify_door1_state();
      display_state();
      while(switch1_pressed()); //Espera a que se deje de pulsar el botón para continuar
    }

  }

  return 0;
}
