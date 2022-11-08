/*
 * RC_Car.c
 * Created: 2022-08-05 오전 11:46:29
 * Author : kcci
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h> // printf , scanf 등이 정의 되어있다.
#include <string.h>//strcpy,strncmp,strcat등이 들어있다.


extern void UART0_transmit(uint8_t data);

//1. for printf
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL , _FDEV_SETUP_WRITE);

uint32_t ms_count=0;  // ms를 재는 변수
uint32_t sec_count=0;  // sec를 재는 변수

extern volatile uint8_t rx_ready_flag; // 완전한 문장이 들어왔을때 1로 set 된다. 

extern void pc_command_processing();
extern void init_led();
extern void init_UART1();

// PORTB.0 : 500ms  on/off
// PORTB.1 : 300ms on/off
// 1. 분주: 64분주 ==> 16000000/64 ==> 250,000HZ
// 2. T 계산 ==> 1/f = 1/250000 = > 0.000004 sec (4us) : 0.004ms
// 3. 8 bit Timer OV: 0.004ms x 256 ==> 0.00124sec ==> 약 1.24ms
//                    0.004ms x 250 ==> 1ms
// 256개의 pulse를 count를 하면 이곳으로 온다.
// 1ms 마다 ISR(TIMER0_OVF_vect) 이곳으로 들어 온다

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6;
	
	ms_count++;
	if (ms_count >= 1000)   // 1000ms ==> 1sec
	{
		ms_count=0;
		sec_count++;    // sec counter증가
	}
}

int main(void)
{ 
	init_led();
	init_button();
	init_uart0(); //uart0를 초기화
	//init_UART1();
	stdout = &OUTPUT; //2. for printf   fprintf(stdout,"test"); ==> printf stdin: 입력(scanf(...))
	init_timer0();	
	sei();							// 전역적으로 인터럽트 허용
	
    while (1) 
    {	
		pc_command_processing();
		//bt_command_processing();
	}
}

void init_timer0(void)
{
	TCNT0=6;   // TCNT 6~256 ==> 정확히 1ms 마다 TIMT 0 OVF INT
	// 분주비를 64로 설정 P269 표13-1
	TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00);

	TIMSK |= (1 << TOIE0);			// 오버플로 인터럽트 허용
	
}