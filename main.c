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


//Defines the possible states of the program for more readability
typedef enum {
  STATE_SET_COUNT = 0,
  STATE_SET_ALARM = 1,
  STATE_COUNTING = 2
} ProgramState;

volatile int button_pressed = 0; //Variable that indicates that a button has been pressed (1)

ProgramState currentState = STATE_SET_COUNT; //Current state of the program

volatile int count = 0; //Current count on the timer (volatile for the loop)
int alarm = 0; //Value of count when the "alarm" will start

int clock_running = 1; //Indicates if the count is currently running (1) or paused (0)


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

void led_red_toggle()
{
    GPIOE->PTOR |=GPIO_PTOR_PTTO(1 << 29);
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

void led_green_toggle()
{
    GPIOD->PTOR |=GPIO_PTOR_PTTO(1 << 5);
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


//TPM CLOCK
void tpm_clock_init(void){

  SIM->SCGC6 |= SIM_SCGC6_TPM0(1); //Activate TPM0 clock
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(3); //Source: MCGIRCLK (already activated)

  TPM0->SC |= TPM_SC_PS(7); //7 -> Prescaler = 128, highest possible value
  TPM0->SC |= TPM_SC_CPWMS(0); //Sets the mode to up-counting
  TPM0->SC |= TPM_SC_TOIE(1); //Enables interrupts for the TPM 0 clock
  TPM0->SC |= TPM_SC_CMOD(1); //Enables TPM counter

  TPM0->MOD = 249; //Modulo needed for a final frequency of 1 Hz (explained in readme)

  NVIC_EnableIRQ(TPM0_IRQn);
}


//TPM0 interrupt handler:
void FTM0IntHandler(void){
  TPM0->SC |= TPM_SC_TOF(1);
    
      if (count > 0) {
          count--;
          lcd_display_time(alarm, count);
      }

      if(count <= alarm){ //Blink in case count is lower than alarm
        led_green_toggle();
        led_red_toggle();
      }
}


//Port C and D interrupt handler:
void PORTDIntHandler(void) {
  
  if (PORTC->ISFR & (1 << 3)) { //Right switch pressed -> Next state
      PORTC->ISFR |= (1 << 3);  // Clear interrupt flag

      if(currentState < STATE_COUNTING){
        currentState ++; //Go to next state
      }

  }

  if (PORTC->ISFR & (1 << 12)) { //Left switch presssed -> Increase starting time/alarm time
      PORTC->ISFR |= (1 << 12);  // Clear interrupt flag
   
      switch (currentState){
        case STATE_SET_COUNT:
            count ++;
            count %= 100;
            lcd_display_time(alarm, count); //Display alarm and count as a time (alarm:count)
            break;
        case STATE_SET_ALARM:
            alarm ++;
            alarm %= 100;
            lcd_display_time(alarm, count); //Display alarm and count as a time (alarm:count)
            break;
        case STATE_COUNTING:
            clock_running = !clock_running;
            TPM0->SC = (TPM0->SC & ~TPM_SC_CMOD_MASK) | TPM_SC_CMOD(clock_running); //Enables or disables the count
            break;
      
      default:
        break;
      }
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
  lcd_display_time(alarm, count); //Display alarm and count as a time (alarm:count)

  SIM->COPC = 0; //Disable Watchdog

  //Enable LEDs
  led_green_init();
  led_red_init();

  //Enable switches
  right_switch_init();
  left_switch_init();

  NVIC_EnableIRQ(PORTC_PORTD_IRQn); //Count ended. Disable buttons

  //Set and clear the corresponding digits to simulate blinking, this indicates to user which number they are modifying
  while(currentState == STATE_SET_COUNT){
    lcd_clear(3);
    lcd_clear(4);
    delay();
    lcd_display_time(alarm, count);
    delay();
  }

  while (currentState == STATE_SET_ALARM){
    lcd_clear(1);
    lcd_clear(2);
    delay();
    lcd_display_time(alarm, count);
    delay();
  }

  if(alarm > count){
    lcd_display_error(0x02);
    return;
  }

  tpm_clock_init();

  //Loop where the count will diminish
  while (count > 0){
    __WFI(); //Wait for interruptions
  }  

  TPM0->SC = (TPM0->SC & ~TPM_SC_CMOD_MASK) | TPM_SC_CMOD(0);
  NVIC_DisableIRQ(PORTC_PORTD_IRQn); //Count ended. Disable buttons
  NVIC_DisableIRQ(TPM0_IRQn);  //Count ended, disable timer interrupts

  led_green_set();
  led_red_set(); //Turn leds off in case they're on

  //Make last number blink again
  while(1){
    lcd_clear(3);
    lcd_clear(4);
    delay();
    lcd_display_time(alarm, count);
    delay();
  }


  return 0;
}
