#include"stdint.h"
#include"tm4c123gh6pm.h"

#define OutputPin 1

void send_data(char);
char read_data(void);



// Let the switch be SW1 in PORTF

// Let the led on PIN1 of PORTF

// i used the temperature sensor in tm4c123gh6pm



char led_on = 'f' ; // Led in initially OFF


void Portf_init(void)
{
	//==================== WHEN PRESS ON SWITCH 2 LED RED TOGGLES ====================//
	
	SYSCTL_RCGCGPIO_R |= 1<<5  ;       // Activate PORTF
	
	NVIC_EN0_R |= 1<<30 ;             // Enable interrupt of PORTF
	NVIC_PRI8_R = (NVIC_PRI8_R&0x0FFFFFFF) | 0x400000000 ;  // PORTF has irq of priority 4

	GPIO_PORTF_LOCK_R = 0x4c4f434b ;  // Unlocking PORTF
	GPIO_PORTF_CR_R = 0x1f ;          // Committing PORTF
	
	GPIO_PORTF_DIR_R &= (~(1<<4));       // Set PIN4 (Switch 2 ) as input
	GPIO_PORTF_DIR_R |= 1<<OutputPin | (1<<2);        // Set PIN1 (red led) as output
	GPIO_PORTF_DEN_R |= (1<<OutputPin | 1<<4 | 1<<2 ) ;   // Set PIN1 and PIN4 as digital output and input respectively 
	GPIO_PORTF_PUR_R |= 1<<4 ;	

	GPIO_PORTF_IM_R = ~(1<<4) ;  // Disable interrupts on PIN4 until adjustment 
	GPIO_PORTF_IS_R |= (1<<4) ;   // Setting interrupt controller to sense levels
	GPIO_PORTF_IBE_R &= ~(1<<4) ; // Setting interrupt not to be caused on both levels 
	GPIO_PORTF_IEV_R &= ~(1<<4);   // Setting interrupt to be caused on low levels
	GPIO_PORTF_IM_R = 1<<4;      // Enable interrupt of PIN4 (switch 1)
}


void timer0_module_a_init(void)
{
	//==================== ADJUST TIMER SO EVERY THREE SECONDS AN INTERRUPT IS CAUSED AND READS VALUE OF SENSOR ====================//

	SYSCTL_RCGCTIMER_R |= 1;  // Activate TIMER0
	TIMER0_CTL_R &= 0 ;  // Disable module A of TIMER0 until adjustment
	TIMER0_CFG_R |= 0X0;     // Set TIMER0 as 32-bit timer
	TIMER0_TAMR_R |= 0x02 ; // Set module A of TIMER0 as periodic and count down timer
	TIMER0_TAILR_R = 48000000 ; // Timer is reloaded by three seconds (frequency of board is 16 MHZ ) 
	//TIMER0_IMR_R |= 0X1 ;  // Enble interrupt of TIMER A
	TIMER0_TAPR_R = 0X0 ;  // Prescalar is 0
	TIMER0_CTL_R |= 0x21 ;    // Enable module A of TIMER0
	
	NVIC_EN0_R |= (1<<19) ; // Enable interrupt of TIMER0 A
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x0fffffff)|0x30000000 ; // TIMER0 A has interrupt priority = 3
}

void adc0_init(void)
{
	//==================== ANAOLOG INPUT 0 (ADC0) IS PIN3 PORTE ====================//

	SYSCTL_RCGCGPIO_R |= 1<<4  ;  
	GPIO_PORTE_LOCK_R = 0X4c4f434b ; //Unlocking PORTE
	GPIO_PORTE_CR_R = 0Xff ;  // Committing PORTE
	GPIO_PORTE_DIR_R &= ~(1<<3); // Set PIN3 as input
	GPIO_PORTE_DEN_R &= ~(1<<3) ; 
	GPIO_PORTE_AMSEL_R |= 1<<3 ;  // Set PIN3 as analog input
	
	//==================== ENABLE ADC0 AND MAKE IT TIMER0 TRIGGERED ====================//

	SYSCTL_RCGCADC_R |= 1; // Enable ADC0
	// There must be 3 clock cycles of delay
	NVIC_EN0_R |= 1<<30 ;   // Enable interrupt of PORTF
	NVIC_EN0_R |= 1<<17 ; // Enable interrupt of ADC0
	NVIC_PRI8_R = (NVIC_PRI8_R&0x0FFFFFFF) | 0x400000000 ;  // PORTF has irq of priority 4
	NVIC_PRI4_R = (NVIC_PRI4_R & 0xffffff0f) | 0x00000020 ; // ADC irq has priority of 2

	ADC0_ACTSS_R &= ~(1<<3); // Disable ss3 until adjustment
	ADC0_EMUX_R = 0x5000; // Select trigger for ss3 to processor
	ADC0_SSMUX3_R = 0x1 ;  // Specify that input to ss3 is PIN 3E (AI1 -> Analog input 1)
	ADC0_SSCTL3_R  = 0xe ; // Specify end bit to prevent unpredictable behaviour
	ADC0_IM_R |= 1<<3 ; // Enable interrupt of ss3
	ADC0_ACTSS_R |= (1<<3); // Activate ss3
}

void uart0_init(void)
{
	//==================== ENABLE UART0 ====================//

	SYSCTL_RCGCUART_R |= 1 ; //Enable UART0
	SYSCTL_RCGCGPIO_R |= 1<<0 ;
	GPIO_PORTA_AFSEL_R |= (0x03) ; // Set PINS 0 and 1 to use alternative functions
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xffffff00)| 0x00000011 ; // Set PIN0 as RX and PIN1 as TX
	GPIO_PORTA_DEN_R |= 0x03 ;
	UART0_CTL_R = 0 ; // Disable UART0 until adjustment
	UART0_IBRD_R = 104 ; // Set integer baud rate divisor  //347
	UART0_FBRD_R = 11 ; // Set float baud rate divisor     // 15
	UART0_LCRH_R = (1<<5) | (1<<6) ; // Enable FIFO and setting data bits to 8 bits
	UART0_CTL_R = 0x301 ; //Enable UART0 , RX and TX
	UART0_CC_R = 0x00 ; // Clock for UART0 is system clock divided by 16
}


void  ADC0SS3_Handler (void)
{
	
	ADC0_ISC_R = 1<<3 ;
	uint32_t temp = 0 ;
	temp = ADC0_SSFIFO3_R ; 
	temp = 147.5 - (247.5*temp)/4096 ; // temp is the Temperature now in degree celasius
	//GPIO_PORTF_DATA_R ^= 1<<2 ;
}

void GPIOF_Handler(void)
{
	GPIO_PORTF_ICR_R = 1<<4 ;
	if(led_on == 'f')
	{
		GPIO_PORTF_DATA_R |= 1<<OutputPin ; 
		led_on = 't' ;
		// Show on terminal
		send_data('O') ;
		send_data('N');
	}
	else
	{
		GPIO_PORTF_DATA_R &= ~(1<<OutputPin) ; 
		led_on = 'f' ;
		// Show on terminal
		send_data('O');
		send_data('F');
		send_data('F');
	}
	send_data('\n');
}

void send_data(char data)
{
	while( (UART0_FR_R & 0x20) != 0 );
	//GPIO_PORTF_DATA_R = 1<<2 ;
	UART0_DR_R = data ;
}

char read_data(void)
{
	while( (UART0_FR_R & 0x10) != 0 ) ;
	return ((char)UART0_DR_R);
}
