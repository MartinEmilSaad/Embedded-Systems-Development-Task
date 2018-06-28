# Embedded-Systems-Development-Task
This task was performed and tested using microcontroller _Ttm4c123gh6pm_ of the **TIVA C 123** series.  
I used **KEIL v5** to write and run my c code.  
You need a _tm4c123gh6pm_ microcontroller in order to test this task.  
You will also need **PUTTY** program for serial communication between board and PC using _UART_.  
Once you upload the code and open the **PUTTY** program every 3 seconds the temperature measured by built in temperature sensor will be shown on the serial , also each time press on _push button_ SW1 the red led of the microcontroller will toggle and the led's status will be shown on the serial either **ON** or **OFF**.  
If you just want to test this task alone you need to copy and paste all the files except _main.c_ .  
If you want to have this task among with other tasks you can copy and paste all the files except _testing_task.c_.  

**## Assumptions made**  
I assumed that input pin is the SW1 _found on PORTF pin4_ and the output pin to be PF1 _red led_.     
I assumed that the frequency of the micrcontroller to be 16 Mhz.  
You could change the output pin by changing the value of OutputPin in the #define OutputPin 1  
I didn't use an extra temerature sensor,i used the one which is already found in the TIVA and used ADC0 to read it and convert it to digital.  
I used GPTM0 (_general purpose timer 0_) and made ADC0 timer triggered. 
