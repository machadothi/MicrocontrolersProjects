
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

uint32_t valor[8];
int freq=10000;
float duty_c = 0.5;

uint32_t ui32SysClock;

int cont =0;

void call_adc (){

    cont++;
    if(ADCIntStatusEx(ADC0_BASE,true) & ADC_INT_SS0){
            ADCIntClearEx(ADC0_BASE, ADC_INT_SS0);
            ADCSequenceDataGet(ADC0_BASE, 0, valor);
            TimerMatchSet(TIMER0_BASE, TIMER_A, (valor[0]/4095.0)*(ui32SysClock/freq));

        }

}

int
main(void)
{


    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                           SYSCTL_OSC_MAIN |
                                           SYSCTL_USE_PLL |
                                           SYSCTL_CFG_VCO_480), 120000000);
    //
    // Run from the PLL at 120 MHz.
    //


    //
    // Enable and wait for the port to be ready for access
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }
    //PWM
    GPIOPinConfigure(GPIO_PD0_T0CCP0);
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    //setup adc
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(TIMER0_BASE,(TIMER_CFG_A_PWM | TIMER_CFG_B_PERIODIC | TIMER_CFG_SPLIT_PAIR));
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, ui32SysClock/freq);
    //TimerMatchSet(TIMER0_BASE, TIMER_A, duty_c*(ui32SysClock/freq)); //25% of 10k
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
    TimerControlTrigger(TIMER0_BASE, TIMER_B, true);

    //adc config
    ADCClockConfigSet(ADC0_BASE, (ADC_CLOCK_SRC_PLL|ADC_CLOCK_RATE_FULL), 16);

    ADCSequenceConfigure(ADC0_BASE,    0,   ADC_TRIGGER_TIMER, 0);

    ADCSequenceStepConfigure(ADC0_BASE,  0,  0,(ADC_CTL_IE|ADC_CTL_END|ADC_CTL_CH0));
    ADCSequenceEnable(ADC0_BASE, 0);



    //CONFIGURE INTERRUPT
    ADCIntRegister(ADC0_BASE, 0, call_adc);
    ADCIntEnableEx(ADC0_BASE, ADC_INT_SS0);
    //
    // Loop Forever
    //

    while(1)
    {

    }
}
