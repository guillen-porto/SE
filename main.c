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
                                TSI_GENCS_MODE(0u) |  // Set at 0 for capacitive sensing.  Other settings are 4 and 8 for threshold detection, and 12 for noise detection
                                TSI_GENCS_REFCHRG(0u) | // 0-7 for Reference charge
                                TSI_GENCS_DVOLT(0u) | // 0-3 sets the Voltage range
                                TSI_GENCS_EXTCHRG(0u) | //0-7 for External charge
                                TSI_GENCS_PS(0u) | // 0-7 for electrode prescaler
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

    TPM_GetDefaultConfig(&tpmInfo);
    /* Initialize TPM module */
    TPM_Init(BOARD_TPM_BASEADDR, &tpmInfo);

    TPM_SetupPwm(BOARD_TPM_BASEADDR, tpmParam, 2U, kTPM_EdgeAlignedPwm, 24000U, TPM_SOURCE_CLOCK);
    TPM_StartTimer(BOARD_TPM_BASEADDR, kTPM_SystemClock);
}





int main(void)
{
    PWM_Init();
    TSI_Init();

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */

    /* Print a note to terminal */
    PRINTF("\r\nTPM example to output PWM on 2 channels\r\n");
    PRINTF("\r\nIf an LED is connected to the TPM pin, you will see a change in LED brightness if you enter different values");
    PRINTF("\r\nIf no LED is connected to the TPM pin, then probe the signal using an oscilloscope");

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
     */
    while (1)
    {
        do
        {
            PRINTF("\r\nPlease enter a value to update the Duty cycle:\r\n");
            PRINTF("Note: The range of value is 0 to 9.\r\n");
            PRINTF("For example: If enter '5', the duty cycle will be set to 50 percent.\r\n");
            PRINTF("Value:");
            getCharValue = GETCHAR() - 0x30U;
            PRINTF("%d", getCharValue);
            PRINTF("\r\n");
        } while (getCharValue > 9U);

        updatedDutycycle = getCharValue * 10U;

        /* Start PWM mode with updated duty cycle */
        TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)LED_GREEN_CHANNEL, kTPM_EdgeAlignedPwm,
                               updatedDutycycle);
        TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)LED_RED_CHANNEL, kTPM_EdgeAlignedPwm,
                               updatedDutycycle);

        PRINTF("The duty cycle was successfully updated!\r\n");
    }
}
