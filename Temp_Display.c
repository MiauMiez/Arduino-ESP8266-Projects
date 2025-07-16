/* 
 * File:   Temp_Display.c
 * Author: alhu
 *
 * Created on 11 June 2025, 14:08
 */
/*
 * Temperaturmessung für das Projekt "Klimaschule"
 */ 


//#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "Temp_Display.h"

FUSES =
{
.low = (0xCF),
.high = (0x99) 
}; 
    

int main(void)
{
	WDTCR = 0x1F;	//Watchdog off
	WDTCR = 0x17;	//Watchdog off
	DDRB = 0b11111111; //Init Port B
	PORTB = 0b01100000;
	DDRD = 0b11110010;	//Init Port D
	PORTD = 0b00001111;
	DDRC = 0b11000011;		//Init Port C
	PORTC = 0;
	DDRA = 0x00;		//Init Port A
	PORTA = 0x00;
	UBRRL = 7;	//Baud rate = 115200 bei 14,7456 MHz
	UCSRA = 0;	//Normal Baud rate (not double speed)
	UCSRB = 0b11011000;	//Uart Controll 
	// **** Timer2 init, Timer 10ms
	ASSR = 0;
	TCCR2 = 0b00001111;	//Timer CLK on Prescaler CK/1024, CTC Mode
	OCR2 = 143;			//Timer mit divisor laden
	TIMSK = 0b10011010; //Interrupt Mask enable Timer-Interrupt
	MCUCR = 0b00001010; //Int0 falling edge,Int1 falling edge
	GICR = 0b11000000; //External Int Enable
		delay_ms	(2000); 
                led_off;
                 counter_1s = 0;
                loop_counter = 0;
	start_adc ();
    
    delay_ms(2000);
//
	sei();
//	cli();// IRQs enable
    while(1)
	{
        temp[0]=adc_value[0]/2-273;
        temp_k[0]=(adc_value[0]&1)*5;
        
        temp[1]=adc_value[1]/2-273;
        temp_k[1]=(adc_value[1]&1)*5; 
        
        temp[2]=adc_value[2]/2-273;
        temp_k[2]=(adc_value[2]&1)*5; 
        
        temp[3]=adc_value[3]/2-273;
        temp_k[3]=(adc_value[3]&1)*5; 
        
        temp[4]=adc_value[4]/2-273;
        temp_k[4]=(adc_value[4]&1)*5;
        
        temp[5]=adc_value[5]/2-273;
        temp_k[5]=(adc_value[5]&1)*5;
       
        
 //     if (flag_minute)
        {
            print_data ();
            flag_minute = 0;
        }
        delay_ms (800);
        led_green;
        delay_ms (200);
        led_off;
 
	}
}


void print_data()
{
        sprintf(buffer,"%d,%u;%d,%u;%d,%u;%d,%u;%d,%u;%d,%u;   \r\a",
        temp[0], temp_k[0], 
        temp[1], temp_k[1],   
        temp[2], temp_k[2],    
        temp[3], temp_k[3],            
        temp[4], temp_k[4],
        temp[5], temp_k[5]); //Data to RS232
        
        start_rs232();
        delay_ms (20);
}

void start_adc () //starts the transmitting at RS232
{
	SFIOR = 0;		//ADC free running mode
	ADMUX = cha | 0b11000000;	//ADC Kanal auswählen mit Internal 2,56V ref. right adjust Single ended input
	ADCSRA = 0b11001111;	//ADC init mit Prescale /128
}

void delay_time_base(uint16_t ms) // wartet x ms Millisekunden
{
	unsigned int t;
  for (t=1; t<=ms; t++)
  	{
	delay_1ms();
	if (rs232_count != 99) break; 
	}
}

void delay_1ms() //warte 1ms bei einer Taktfrequenz von 14,7456MHz
{
	volatile unsigned int t, x;
	x=0;
  for (t=1; t<=475; t++)
	  x= x+1;
}

void delay_ms(uint16_t ms) //delay routine 1ms ... 65,535s
{
	uint16_t t,t1;
        volatile uint8_t dummy;
	//
	for (t=0; t<=ms; t++)
	{
		for (t1=1; t1<=1620; t1++)
		{ ++dummy;}
	}

}

void delay_us(uint16_t us) //delay routine 1us ... 32,767ms
{
	uint16_t t;
	volatile uint8_t dummy;
	//
//	us = us*2;
	for (t=0; t<=us; t++)
	{
	++dummy;
	++dummy;
	}
}


ISR (INT0_vect) //Taster S1
{
	delay_ms (50);
	if ((PIND & 0x04) == 0) //Taster entprellung d.h. ist der Taster nach 50ms noch immer gedrückt ?
	{
        print_data ();
	}
}

ISR (INT1_vect) //Taster S2
{
	delay_ms (50);
	if ((PIND & 0x08) == 0) //Taster entprellung d.h. ist der Taster nach 50ms noch immer gedrückt ?
	{
        print_data ();
	}
}



ISR (TIMER2_COMP_vect) //10ms
{
 //   counter_time_stamp+=10; //Counter in ms // jetzt in Sekundentakt
	if (timeout) timeout--;

    counter_10ms--;
	if (!(counter_10ms)) //one Secound clock
	{
		counter_10ms = 100;
		--counter_1s;
		counter_time_stamp++; //Counter in s
        if (!(counter_1s)) //1 Minuten Clock
		{
                    counter_1s=60;
                    --counter_minute;
                    flag_minute=1;
//                    speed_sum += speed_val;
                    if (!counter_minute) counter_minute=60;

                    
		}
	}
}


void start_rs232 () //starts the transmitting at RS232
{
//	led_green;
	UDR = buffer[0]; //Output the first two signs
//	UDR = buffer[1];
	buffer_pointer = 1;	//Setting the pointer for the interrupt-routine
	enable_dataempty_int; //Enable interrupt
}
//
ISR (USART_UDRE_vect) //Dataregister empty interrupt RS232
{
	if (buffer[buffer_pointer]!=7) //Chr$ 0x07 = list-end-sign
	{
		UDR = buffer[buffer_pointer];
		buffer_pointer++;
	}
	else
	{
		disable_dataempty_int;
		enable_dataready_int;
//		led_off;
	}
}
//
ISR (USART_TXC_vect) //Data send ready RS232
{
	disable_dataready_int; //disable intterrupt send ready
	led_off;
}
//

ISR (USART_RXC_vect) //Data received on RS232
{
	uint8_t y;
	uint16_t intputdata;
	y=UDR;

	switch (y)
	{
	case 'r': //Reset Timestamp with "r"
		time_stamp = 0;
        counter_time_stamp = 0;
		if (rs232_count == 101)	--rs232_count;
		break;
//	case 't': case 'l': case 'L': case 'p'://Start of input
    case 't':
		rs232_count = 0; 
		nu_input = 0;
		fl_input = 1;
		buffer_input[rs232_count] = y;
		rs232_count++;
		break;
	case 'b': //or with "b" = braek
		rs232_count = 100;
		break;
	case 'c': //Continue with "c"
		rs232_count = 99;
		break;
	case 'h': //Help with "h"
		rs232_count = 110;
		break;


	default:
		if (rs232_count < 99)
		{
			buffer_input[rs232_count] = y;
			rs232_count++;
			if (rs232_count >= 18) rs232_count = 0; //buffer overrun
			if (y==13) //CR end of datas
			{
				rs232_count = 110;	// help and than mode run = 99
				fl_input = 0;		//clear flag number input
			}
			if (fl_input == 1 && rs232_count == 2) //time_base
			{
				nu_input =  (buffer_input[1]-48);	
			}
			if (fl_input == 1 && rs232_count == 3) //time_base
			{
				intputdata = (buffer_input[1]-48)*10;
				intputdata = intputdata+ (buffer_input[2]-48);
				nu_input =  intputdata;	
			}
			if (fl_input == 1 && rs232_count == 4) //time_base
			{
				intputdata = (buffer_input[1]-48)*100;
				intputdata = intputdata+ (buffer_input[2]-48)*10;
				intputdata = intputdata+ (buffer_input[3]-48);
				nu_input =  intputdata;	
			}
			if (fl_input == 1 && rs232_count == 5) //time_base
			{
				intputdata = (buffer_input[1]-48)*1000;
				intputdata = intputdata+ (buffer_input[2]-48)*100;
				intputdata = intputdata+ (buffer_input[3]-48)*10;
				intputdata = intputdata+ (buffer_input[4]-48);
				nu_input =  intputdata;	
			}
			if (fl_input == 1 && rs232_count == 6) //time_base
			{
				intputdata = (buffer_input[1]-48)*10000;
				intputdata = intputdata+ (buffer_input[2]-48)*1000;
				intputdata = intputdata+ (buffer_input[3]-48)*100;
				intputdata = intputdata+ (buffer_input[4]-48)*10;
				intputdata = intputdata+ (buffer_input[5]-48);
				nu_input =  intputdata;	
			}
		}
	}
}
//
ISR (ADC_vect) //ADC ready
{
	adc_value[cha] = ADC;
	if (cha<=6) cha++; else cha=0;
	ADMUX = cha | 0b11000000;	//ADC Kanal auswählen mit Internal 2,56V ref. right adjust Single ended input
	ADCSRA = 0b11001111;	//ADC init mit Prescale /128
}