// Connect a POT betweek 0 and 3.3V with the wiper connected to PA0
// NOTE!!: PA0 actually connects to IN5 of ADC1.
// PA2 is used for USART 
#include <stm32l432xx.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include "eeng1030_lib.h"
#include "circular_buffer.h"

void setup(void);
void delay(volatile uint32_t dly);
void initTimer1(void);
//void initTimer2(void);
void initSerial(uint32_t baudrate);
void initEncoder(void);
void eputc(char c);
int currentTimestamp = 0;
int oldTimestamp = 0;
int duration = 0;
int period = 0;
int vin;
int currentSpeed = 0;
volatile uint32_t pulseTime = 0;
volatile uint32_t rpm = 0;
int count;
int oldSpeed = 0;
circular_buffer rx_buf;
volatile uint32_t data_ready=0;
static int button_pressed = 0;


int main()
{
    setup();   
    initEncoder();    
    while(1)
    {

        while(!(TIM1->SR & TIM_SR_CC2IF)) //while flag is low do nothing  
        {
         printf("FAN OFF\r\n",TIM1->CNT);
        }
        
        //encoder value making it between 0 an 1000
        if (TIM2->CNT < 0) TIM2->CNT = 0;
        if (TIM2->CNT > 1000) TIM2->CNT = 1000;

        int encoder_val = TIM2->CNT;
        int duty =  encoder_val / 10; 

        setPWMDuty(100 - duty); //seems like invertedd control
       
        captureFanSpeed(); 
    

        printf("Duty: %d%% | Encoder: %d | RPM: %d\r\n", duty, encoder_val, rpm);

        if ((GPIOB->IDR & (1 << 4)) == 0)  // Button pressed (active LOW)
        {
            printf("lOW POWER MODE!\r\n");
            setPWMDuty(80);
            delay_ms(2000000);
           
        }


        delay(10000000); 
    
    }

}


void setup(void)
{
    RCC->AHB2ENR |= (1 << 0) | (1 << 1); // turn on GPIOA and GPIOB

    pinMode(GPIOB,3,1);                     // PB3 digital output - LED
    pinMode(GPIOA,3,2);  // alternative function mode for PA3
    pinMode(GPIOA,2,2); // alternate function mode for PA2
    pinMode(GPIOB,4,0); // Switch
    enablePullUp(GPIOB,4);
   // pinMode(GPIOA,15,2);//
       
   // selectAlternateFunction(GPIOA,15,3); // AF3 = USART2 RX PA15
    selectAlternateFunction(GPIOA,2,7); // AF7 = USART2 TX PA2
    
    
    initTimer1();                           // Initialise timer 1
   // initTimer2();                           // Initialise Timer 2
    // NVIC->ISER[0] |= (1 << 28);             // IRQ 28 maps to TIM2 (Alternative NVIC_EnableIRQ(TIM2_IRQn))
    // TIM2->DIER |= (1 << 1);                // Enable interrupt on CC1
    // __enable_irq();                         // __asm(" cpsie i ");

    initClocks(); 
    SystemCoreClock = 80000000;   
    SysTick->LOAD = 80000-1; // Systick clock = 80MHz. 80000000/80000=1000
	SysTick->CTRL = 7; // enable systick counter and its interrupts
	SysTick->VAL = 10; // start from a low number so we don't wait for ages for first interrupt
	__asm(" cpsie i "); // enable interrupts globally
    RCC->AHB2ENR |= (1 << 0) + (1 << 1); // enable GPIOA and GPIOB
    
    
    initSerial(9600);                       // Initialise serial comms

}


void delay(volatile uint32_t dly)
{
    while(dly--);
}
void initTimer1(void)
{
    // Enable GPIOA and TIM1
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Configure PA8 as alternate function (AF1 = TIM1_CH1)
    GPIOA->MODER &= ~(3 << (8 * 2));
    GPIOA->MODER |= (2 << (8 * 2));
    GPIOA->AFR[1] &= ~(0xF << ((8 - 8) * 4));
    GPIOA->AFR[1] |= (1 << ((8 - 8) * 4)); // AF1 = TIM1_CH1

    //Configure PA9 as alternate function (AF1 = TIM1_CH2, pls work)
    GPIOA->MODER &= ~(3 << (9 * 2));
    GPIOA->MODER |= (2 << (9 * 2));
    GPIOA->AFR[1] &= ~(0XF << ((9-8) * 4));
    GPIOA->AFR[1] |= (1 << ((9-8) * 4)); //AF1 = TIM1_CH2

    // Set up TIM1 CH1 in PWM mode
    TIM1->PSC = 80 - 1;     // Prescaler (assuming 80MHz clock → 1MHz timer)
    TIM1->ARR = 1000 - 1;   // 1 kHz PWM (1ms period)
    TIM1->CCR1 = 800; // 80% of ARR = 1000

    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1->CCMR1 |= (7 << 4);  // PWM Mode 2 (Inverted)
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload

    TIM1->CCER |= TIM_CCER_CC1E;  // Enable output on CH1

    TIM1->BDTR |= TIM_BDTR_MOE;   // Enable main output (advanced timer)

    TIM1->CR1 |= TIM_CR1_ARPE;    // Enable auto-reload preload
    TIM1->EGR |= TIM_EGR_UG;      // Force update
    TIM1->CR1 |= TIM_CR1_CEN;     // Enable timer

    // Set up TIM1 CH2 for input capture (tach)
    TIM1->CCMR1 &= ~TIM_CCMR1_CC2S;
    TIM1->CCMR1 |=  (1 << 8);      // CC2 mapped to TI2
    TIM1->CCER  &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP); // rising edge
    TIM1->CCER  |=  TIM_CCER_CC2E; // enable capture

}


void setPWMDuty(int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100; //making duty cycle between 0 and 100%

    //int physPercent = 50 + (percent * 100) / 100;

    //TIM1->CCR1 = (physPercent * (TIM1->ARR + 1)) / 100;
    TIM1->CCR1 = (percent * (TIM1->ARR + 1)) / 100;

}

void initSerial(uint32_t baudrate)
    {
        RCC->APB1ENR1 |= (1 << 0); // make sure GPIOA is turned on
        const uint32_t CLOCK_SPEED= SystemCoreClock;    
        uint32_t BaudRateDivisor;
        BaudRateDivisor = CLOCK_SPEED/baudrate;	
        RCC->APB1ENR1 |= (1 << 17); // turn on USART2 (14 for USART )
        
        USART2->CR1 = 0;
        USART2->CR2 = 0;
        USART2->CR3 = (1 << 12); // disable over-run errors
        USART2->BRR = BaudRateDivisor;
        USART2->CR1 =  (1 << 3) | (1 << 2);  // enable the transmitter and receiver
        USART2->CR1 |= (1 << 5); // enable receiver interrupts
        USART2->CR1 |= (1 << 0); // enable the UART
        USART2->ICR = (1 << 1); // clear any old framing errors
        
        
    }


void initEncoder()
{
    RCC->AHB2ENR |= (1 << 0) | (1 << 1); // make sure GPIOA,B are turned on   
    pinMode(GPIOA,0,2);
    selectAlternateFunction(GPIOA,0,1);
    pinMode(GPIOB,3,2);
    selectAlternateFunction(GPIOB,3,1);
    pinMode(GPIOB,4,0);
    RCC->APB1ENR1 |= (1 << 0); // enable Timer 2
    TIM2->ARR = 0xffff; //max value
    TIM2->SMCR = 0b010;
    TIM2->CCMR1 = (2 << 8)+(1 << 0)+ (0x0f<<4)+ (0x0f<<12);
    TIM2->CR1 |= 1;

}

int captureFanSpeed()
    {
        // Calculate fan speed based on captured pulse time from FG signal (PA9)
        if (TIM1->SR & TIM_SR_CC2IF)  // Check if capture event occurred (CC1IF flag)
        {
            currentSpeed = TIM1->CCR2; // directly from tach
            duration = currentSpeed - oldSpeed; //period
            

            
            period = 1000*duration/SystemCoreClock;
            rpm = (60000000) / (duration * 2); // 2 pulses per rev, timer at 1MHz
            oldSpeed = currentSpeed;

            //if (rpm > 3000)
            //rpm = 3000; 
           //printf("Millesecond rotations: %f\r\n",rpm);

          // TIM1->SR &= ~TIM_SR_CC2IF;

            //return rpm;
        }
    }

    void USART2_IRQHandler()
    {
        char c;
        if ((USART2->ISR &(1<<5)) )
        {
            c = USART2->RDR;
            if (c==']')
            {
                data_ready = 1;
            }
            else if(c=='[')
            {
                init_circ_buf(&rx_buf);
            }
            else
                put_circ_buf(&rx_buf,c);
        }
    }

    int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }
    while(len--)
    {
        eputc(*data);    
        data++;
    }    
    return 0;
}
void eputc(char c)
{
    while( (USART2->ISR & (1 << 6))==0); // wait for ongoing transmission to finish
    USART2->TDR=c;
}       




