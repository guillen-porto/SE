#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_tpm.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The Flextimer instance/channel used for board */
#define BOARD_TPM_BASEADDR TPM0
#define LED_GREEN_CHANNEL 2U
#define LED_RED_CHANNEL 5U

/* Get source clock for TPM driver */
#define TPM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_PllFllSelClk)

#define OFFSET 1000
#define MIN_VAL 500

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint8_t getCharValue = 0U;
volatile uint8_t updatedDutycycle = 10U;

/*******************************************************************************
 * Code
 ******************************************************************************/


// TSI initialization function
void TSI_Init(void)
{
    // Enable clock for TSI PortB 16 and 17
    SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;
     
     
    TSI0->GENCS = TSI_GENCS_OUTRGF_MASK |  // Out of range flag, set to 1 to clear
                                //TSI_GENCS_ESOR_MASK |  // This is disabled to give an interrupt when out of range.  Enable to give an interrupt when end of scan
                                TSI_GENCS_MODE(4u) |  // Set at 0 for capacitive sensing.  Other settings are 4 and 8 for threshold detection, and 12 for noise detection
                                TSI_GENCS_REFCHRG(4u) | // 0-7 for Reference charge
                                TSI_GENCS_DVOLT(1u) | // 0-3 sets the Voltage range
                                TSI_GENCS_EXTCHRG(5u) | //0-7 for External charge
                                TSI_GENCS_PS(2u) | // 0-7 for electrode prescaler
                                TSI_GENCS_NSCN(31u) | // 0-31 + 1 for number of scans per electrode
                                TSI_GENCS_TSIEN_MASK | // TSI enable bit
                                //TSI_GENCS_TSIIEN_MASK | //TSI interrupt is disables
                                TSI_GENCS_STPE_MASK | // Enables TSI in low power mode
                                //TSI_GENCS_STM_MASK | // 0 for software trigger, 1 for hardware trigger
                                //TSI_GENCS_SCNIP_MASK | // scan in progress flag
                                TSI_GENCS_EOSF_MASK ; // End of scan flag, set to 1 to clear
                                //TSI_GENCS_CURSW_MASK; // Do not swap current sources
     
    // The TSI threshold isn't used is in this application
//    TSI0->TSHD =     TSI_TSHD_THRESH(0x0000) |
//                                TSI_TSHD_THRESL(0x0000);
                                 
}

//PWM initialization function
void PWM_Init(void){
    tpm_config_t tpmInfo;
    tpm_chnl_pwm_signal_param_t tpmParam[2];

    #ifndef TPM_LED_ON_LEVEL  
      #define TPM_LED_ON_LEVEL kTPM_LowTrue
    #endif    
    
    /* Configure tpm params with frequency 24kHZ */
    tpmParam[0].chnlNumber = (tpm_chnl_t)LED_GREEN_CHANNEL;
    tpmParam[0].level = TPM_LED_ON_LEVEL;
    tpmParam[0].dutyCyclePercent = updatedDutycycle;

    tpmParam[1].chnlNumber = (tpm_chnl_t)LED_RED_CHANNEL;
    tpmParam[1].level = TPM_LED_ON_LEVEL;
    tpmParam[1].dutyCyclePercent = updatedDutycycle;

    CLOCK_SetTpmClock(1U);


    /*
     * tpmInfo.prescale = kTPM_Prescale_Divide_1;
     * tpmInfo.useGlobalTimeBase = false;
     * tpmInfo.enableDoze = false;
     * tpmInfo.enableDebugMode = false;
     * tpmInfo.enableReloadOnTrigger = false;
     * tpmInfo.enableStopOnOverflow = false;
     * tpmInfo.enableStartOnTrigger = false;
     * tpmInfo.enablePauseOnTrigger = false;
     * tpmInfo.triggerSelect = kTPM_Trigger_Select_0;
     * tpmInfo.triggerSource = kTPM_TriggerSource_External;
     * */
    TPM_GetDefaultConfig(&tpmInfo);
    /* Initialize TPM module */
    TPM_Init(BOARD_TPM_BASEADDR, &tpmInfo);

    TPM_SetupPwm(BOARD_TPM_BASEADDR, tpmParam, 2U, kTPM_EdgeAlignedPwm, 24000U, TPM_SOURCE_CLOCK);
    TPM_StartTimer(BOARD_TPM_BASEADDR, kTPM_SystemClock);
}



//Function to read a channel of the TSI ()
uint16_t read_TSI_channel(uint8_t channel)
{
    int scan;
    TSI0->DATA =     TSI_DATA_TSICH(channel); // Using channel 10 of The TSI
    TSI0->DATA |= TSI_DATA_SWTS(1); // Software trigger for scan

    while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)); //Wait until scan has ended (end of scan flag set to 1)
    scan = TSI0->DATA & TSI_DATA_TSICNT_MASK; //Read the content of the counter in the TSI_DATA register
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; // Reset end of scan flag
     
    //PRINTF("Channel %u: scan: %u", channel, scan);

    scan = scan - OFFSET;
    if(scan < 0) scan = 0; //Don't return negative numbers
    return scan;
}


void get_led_values(uint16_t* green_val, uint16_t* red_val){
    uint16_t chan_9 = read_TSI_channel(9);
    uint16_t chan_10 = read_TSI_channel(10);

    uint16_t total = chan_9 + chan_10;
    
    if(total > MIN_VAL){
        *green_val = (100 *chan_9) / total;
        *red_val = (100 * chan_10) / total;
    }
}


int main(void)
{
    PWM_Init();
    TSI_Init();

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    uint16_t green_led_val = 0;
    uint16_t red_led_val = 0;
    /* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
    while (1)
    {
        get_led_values(&green_led_val, &red_led_val);

        PRINTF("Red: %u, green: %u\n", red_led_val, green_led_val);
        /* Start PWM mode with updated duty cycle */
        TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)LED_GREEN_CHANNEL, kTPM_EdgeAlignedPwm,
                               green_led_val);
        TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)LED_RED_CHANNEL, kTPM_EdgeAlignedPwm,
                               red_led_val);
    }
}
