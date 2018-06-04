#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"

#define CLOCK 120000000
#define freq 10000
#define ENC_ESQ GPIO_PIN_2
#define ENC_DIR GPIO_PIN_3
uint32_t g_ui32SysClock;

/**
 * main.c
*/
float duty_c_esq = 0.95;
float duty_c_dir = 0.95;
int count = 0;
const int disk=20;

void freq_enc () {
    if(){
        count++;
        GPIOIntClear(GPIO_PORTN_BASE, ENC_DIR|ENC_ESQ);
    }
    else if(){


    }
    else{



    }
}



int main(void)
{
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
    SYSCTL_OSC_MAIN |
    SYSCTL_USE_PLL |
    SYSCTL_CFG_VCO_480),
                                        CLOCK);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    //CONFIGURE INTERRUPT
    GPIOIntRegister(GPIO_PORTN_BASE, &freq_enc);
    GPIOIntTypeSet(GPIO_PORTN_BASE, ENC_DIR|ENC_ESQ, GPIO_RISING_EDGE);
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, ENC_DIR|ENC_ESQ);
    GPIOPadConfigSet(GPIO_PORTN_BASE, ENC_DIR|ENC_ESQ, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntEnable(GPIO_PORTN_BASE, ENC_DIR|ENC_ESQ);

    //CONFIGURE GPIO PWM functions
    GPIOPinConfigure(GPIO_PD0_T0CCP0); //left
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);

    GPIOPinConfigure(GPIO_PD1_T0CCP1); //right
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_1);

    //Timer Configure
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM); //INICIA O TIMER SOURCE

    TimerConfigure(TIMER0_BASE,
                   (TIMER_CFG_A_PWM | TIMER_CFG_B_PWM | TIMER_CFG_SPLIT_PAIR));

    //TIMER_A and TIMER_B
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, g_ui32SysClock/freq);


    //defining duty cycle for PWM
    TimerMatchSet(TIMER0_BASE, TIMER_A, duty_c_dir*(g_ui32SysClock/freq));
    TimerMatchSet(TIMER0_BASE, TIMER_B, duty_c_esq*(g_ui32SysClock/freq));

    //Timers enable
    TimerEnable(TIMER0_BASE, TIMER_BOTH);

    //sync timers
    TimerSynchronize(TIMER0_BASE, TIMER_0A_SYNC|TIMER_0B_SYNC);

    while(1){



    }

}
