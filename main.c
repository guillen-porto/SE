#include "MKL46Z4.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>

//Max number of producers and consumers, one variable would be enough, as they are the same, but this way it can be more easily changed
#define MAX_PRODUCERS 5
#define MAX_CONSUMERS 5

#define QUEUE_SIZE 99
#define ITEM_SIZE 1 //Size (in bytes) of an item

uint8_t numProducers = 0; //Number of producers
uint8_t numConsumers = 0; //Number of consumers

//Handlers to be able to delete the producers and consumers (NOT NECESSARY RIGHT NOW)
/*TaskHandle_t producers[MAX_PRODUCERS];
TaskHandle_t consumers[MAX_CONSUMERS];*/

QueueHandle_t queue; //Handler for the queue

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

/*#################################
########  TASK FUNCTIONS   ########
#################################*/

//Function that the producers will execute
void producerTask(void *pvParameters){
  int* id = (int*)pvParameters;
  int val;

  while(1){
    if(*id < numProducers){ //Check if this producer is active
      val = rand() % 100; //This will always be smaller than 128
      xQueueSend(queue, &val, portMAX_DELAY); //The task will try to wait for the maximum time possible
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); //1 second delay between 2 tasks
  }

}

//Function that the consumers will execute
void consumerTask(void *pvParameters){
  int* id = (int*)pvParameters;
  int val;

  while(1){
    if(*id < numConsumers){ //Check if this consumer is active
      xQueueReceive(queue, &val, portMAX_DELAY); //Wait the maximum time possible to get data from queue
    }
    vTaskDelay(pdMS_TO_TICKS(1000)); //1 second delay between 2 tasks
  }
}

//Function to update the LCD (a dedicated thread will be created for this)
void lcdUpdateTask(void *pvParameters) {
  while (1) {
      uint8_t pendingData = uxQueueMessagesWaiting(queue);
      uint8_t displayed = 10 * numProducers + numConsumers;
      lcd_display_time(pendingData, displayed);
      vTaskDelay(pdMS_TO_TICKS(150)); //Update 4 times per second
  }
} 

/*#################################
#######  Switch interrupt   #######
#################################*/


//Port C and D interrupt handler:
void PORTDIntHandler(void) {
  
  if (PORTC->ISFR & (1 << 3)) { //Right switch pressed -> Increase consumers
      PORTC->ISFR |= (1 << 3);  // Clear interrupt flag
      if(numConsumers < 5){
        numConsumers ++; //Modifying this number automatically makes the corresponding consumer know it's not active anymore
      }
      else{
        //Reset consumers to 0
        numConsumers = 0;
      }

  }

  if (PORTC->ISFR & (1 << 12)) { //Left switch presssed -> Increase producers
      PORTC->ISFR |= (1 << 12);  // Clear interrupt flag
    
      if(numProducers < 5){
        numProducers ++; //Modifying this number automatically makes the corresponding  producer know it's not active anymore
      }
      else{
        //Reset pproducers to 0
        numProducers = 0;
      }
  }
}


int main(void)
{
  SIM->COPC = 0; //Disable Watchdog
  irclk_ini(); // Enable internal ref clk to use by LCD
  lcd_ini();   //Initialize LCD


  queue = xQueueCreate(QUEUE_SIZE, ITEM_SIZE); //Initialize shared queue

  if(queue == NULL){
    //Queue was not created, display error
    lcd_display_error(1);
  }

  //Enable switches
  right_switch_init();
  left_switch_init();
  NVIC_EnableIRQ(PORTC_PORTD_IRQn); //Enable switch interruptions


  //Create LCD updating task
  xTaskCreate(lcdUpdateTask, "LCDTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL); //Creates the task that will update the LCD


  //Create producers and consumers

  int producers[MAX_PRODUCERS]; //Each number will be passed to its corresponding producer as an argument
  for(int i = 0; i < MAX_PRODUCERS; i++){

    producers[i] = i;
    xTaskCreate(producerTask, "Producer", configMINIMAL_STACK_SIZE, &producers[i], 1, NULL); 
  }


  int consumers[MAX_CONSUMERS]; //Each number will be passed to its corresponding consumer as an argument
  for(int i = 0; i < MAX_CONSUMERS; i++){

    consumers[i] = i;
    xTaskCreate(consumerTask, "Consumer", configMINIMAL_STACK_SIZE, &consumers[i], 1, NULL); 
  }

  vTaskStartScheduler(); //Start task scheduler

  //Should never reach here
  for(;;);

  return 0;
}