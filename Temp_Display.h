/* 
 * File:   Temp_Display.h
 * Author: alhu
 *
 * Created on 11 June 2025, 14:08
 */




void delay_ms(uint16_t); //delay routine 1ms ... 65,535s
void delay_time_base(uint16_t ms);
void delay_1ms(void);
void delay_us(uint16_t); //delay routine 1us ... 65,535ms
void print_data (); //Datensatz über RS232 ausgeben
void start_rs232 (void);
void start_adc (void);




#define rs_on (PORTB |= (1 << 0)) //Control LCD
#define rs_off (PORTB &= ~(1 << 0))

#define en_on (PORTB |= (1 << 3)) //Control LCD
#define en_off (PORTB &= ~(1 << 3))*/

#define led_off PORTC &= ~(1 << 0 | 1 << 1) //LED aus
#define led_red PORTC |= (1 << 0) ; PORTC &= ~(1 << 1)
#define led_green PORTC |= (1 << 1) ; PORTC &= ~(1 << 0)



#define clr_int0 (GIFR |= (1 << 6)) //Clear Interrupt flag
#define clr_int1 (GIFR |= (1 << 7)) //Clear Interrupt flag
#define disable_dataempty_int 	(UCSRB &= ~(1<<UDRIE))
#define enable_dataempty_int	(UCSRB |= (1<<UDRIE))
#define disable_dataready_int 	(UCSRB &= ~(1<<TXCIE))
#define enable_dataready_int	(UCSRB |= (1<<TXCIE))


volatile char		fl_input= 0; //Flag input number
volatile uint16_t	nu_input= 0; //number input
//
char buffer[140];					//Puffer for RS232 comunication output
char buffer_input[20];			//Puffer for RS232 comunication input
volatile uint16_t	adc_value[8];	//Result of the Analog to Digital Converter (ADC)
volatile char cha;					//Pointer for the slection of the ADC-Channel
volatile char buffer_pointer;		//Pointer Puffer RS232
volatile char rs232_count;			//Pointer Puffer RS232 input
volatile uint32_t time_stamp;		//time counter in ms
//
volatile uint8_t counter_10ms=100; //counts each 10ms runs one secound
volatile uint32_t counter_time_stamp;		//time counter in s
volatile uint8_t counter_1s=1; //counts each 1s runs 10 secound
volatile uint8_t counter_minute=60; //counts each 10s runs 1800 secound
volatile uint16_t timeout; 
uint16_t loop_counter;
int16_t temp[8];
uint8_t temp_k[8];
bool flag_minute;
