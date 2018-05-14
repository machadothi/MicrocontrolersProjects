#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"



#define USER_LED1  GPIO_PIN_0
#define USER_LED2  GPIO_PIN_1


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

uint32_t ui32SysClock;

int sw = 0;
int key = 0;
int period = 120000000;

void OneShotStart (uint32_t period){

    TimerConfigure(TIMER0_BASE,TIMER_CFG_ONE_SHOT);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE,TIMER_A);

}

bool OneShotDone(){

    bool b = TIMER_TIMA_TIMEOUT & TimerIntStatus(TIMER0_BASE, false);
    if (b) TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    return b;
}

void myint () {

    if(GPIOIntStatus(GPIO_PORTJ_BASE,true)&0x01){


            key=1;

        GPIOIntClear(GPIO_PORTJ_BASE,0x03);
    }

}



void SwState (){

    uint8_t chaveopen = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3);
    uint8_t chaveclose = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_3);

    if(chaveclose==0x0)    sw= 1; //closed

    else if(chaveopen==0x0)  sw= 2; //opened

    else sw= 0;
}


void gateOK () {

    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_4, (GPIO_PIN_4|GPIO_PIN_5));

}

void gateBusy (){
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_4, (0x0|GPIO_PIN_5));
}

void gateError(){

    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_4, (0x0|GPIO_PIN_4));

}

void engOpen ( ){
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x0);
}

void engClose( ) {
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_PIN_7);
}

void engOff () {

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0x0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0x0);
}



int main(void)
{



        ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                           SYSCTL_OSC_MAIN |
                                           SYSCTL_USE_PLL |
                                           SYSCTL_CFG_VCO_480), 120000000);

     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
        {
        }
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
        {
        }
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
        {
        }
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
        {
        }
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
        {
        }
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM))
        {
        }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
        {
        }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
        {
        }

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, (GPIO_PIN_4));
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, (GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7));
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, (GPIO_PIN_2));

    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE,(GPIO_PIN_3));
    GPIOPadConfigSet(GPIO_PORTM_BASE,(GPIO_PIN_3), GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE,(GPIO_PIN_3));
    GPIOPadConfigSet(GPIO_PORTN_BASE,(GPIO_PIN_3), GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntRegister(GPIO_PORTJ_BASE, &myint);
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_BOTH_EDGES);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0);

    TimerClockSourceSet(TIMER0_BASE,TIMER_CLOCK_SYSTEM);
    enum estados_e {

        PE_openning,
        PF_openning,
        PA_stopped,
        PE_MP_closing,
        PE_MP_openning,
        PF_stopped,
        PA_closing,
        PE_closing

    }state;



    SwState();
    key=0;

    while(sw==0){

            gateError();
            SwState();
    }

    if (sw==1) state=PF_stopped;
    else state = PA_stopped;


    while(1)
    {
        switch (state) {

        case PE_openning: // GATED HALF-OPENED ENGINE OPENING
            SwState();
            key=0;
            gateBusy();
            engOpen();

            if(key == 1){
              key=0;
               state=PE_MP_closing;
               }

           else if(sw == 1) state = PA_stopped;
           else state =PE_openning;
            break;

        case PF_openning: // GATE CLOSED ENGINE OPENING

            key=0;
            gateBusy();
            engOff();
            SwState();
            if(sw==0) state = PE_openning;
            break;

        case PA_stopped: //GATE OPENED ENGINE STOPED
            key=0;
            gateOK();
            engOff();

            if(key==1 ){
                key=0;
                state = PA_closing;
            }
            break;

        case PE_MP_closing: //GATED HALF-OPENED ENGINE STOPED2CLOSE
            key=0;
            gateBusy();
            engOff();
            if(key==1){
                key=0;
                state=PE_closing;
            }
            break;

        case PE_MP_openning:// GATED HALF-OPENED ENGINE STOPED2OPEN
            key=0;
            gateBusy();
            engOff();
            if(key==1){
            key=0;
            state=PE_openning;
            }
            break;

        case PF_stopped: //GATE CLOSED ENGINE STOPED
            key=0;
            gateOK();
            engOff();
            if(key==1){
            key=0;
            state=PF_openning;
            }
            break;

        case PA_closing:// GATE OPENED ENGINE CLOSING
            key=0;
            gateBusy();
            engOff();
            SwState();
            if(sw==0) state =PE_closing;
            break;

        case PE_closing:// GATED HALF-OPENED
            key=0;
            gateBusy();
            engClose();
            SwState();
            if (key ==1){
                key=0;
                state=PE_MP_openning;
            }
            else if (sw == 1) state = PF_stopped;
            break;

        default:
            gateError();
            break;


        }



    }
}


