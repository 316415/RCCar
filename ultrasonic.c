/*
 * ultrasonic.c
 *
 * Created: 2022-08-10 오후 4:21:54
 *  Author: kccistc
 */ 
#include "ultrasonic.h"

void init_ultrasonic();
void ultrasonic_trigger();

extern uint32_t uled;
volatile int ultrasonic_distance ; //펄스길이 저장 변수
char scm[50]; 

//PE4: 외부 인터럽트 4번 초음파 센서 상승,하강 에지 둘다 이쪽으로 들어온다.
ISR(INT4_vect)//INTERRUPT SERVICE RUTIN
{
	if(ECHO_PIN & (1<<ECHO)) //상승에지
	{
		TCNT1 = 0;
	}
	else // 하강 에지
	{
		//에코핀의 펄스길이를 us 단위로 환산
		ultrasonic_distance = 1000000.0 * TCNT1 *1024 / F_CPU;
		//1cm : 58us 소요
		sprintf(scm , "distance: %dcm", ultrasonic_distance/58); //cm 환산
	}
}


//PG4 :trriger
//PE4 : ECHO (INT4) 외부 인터럽트 4번
void init_ultrasonic()
{
	TRIG_DDR |= (1 << TRIG); //OUTPUT 설정
	ECHO_DDR &= ~(1 << ECHO); //INPUT 설정
	
	//0 1 :어떠한 형태로든지 신호변화가 발생 되면 interrupt 요청 (상승 , 하강 edge 둘다 interrupt)
	EICRB |= (0 << ISC41 ) | (1 << ISC40 );
	TCCR1B |= (1 << CS12) | (1 << CS10 ); // 분주비  = 1024
	//16비트 타이머에서 1번을 1024 분주해서 공급한다.
	//16MHz 를  1024 분주 --> 16000000/1024 == 15625Hz == 15.625KHz
	//1펄스의 길이 : t = 1/f  대입시 =>  1/15625 ==> 0.000064sec ==> 64us
	
	EIMSK |= (1 << INT4); //외부 INT4(ECHO) 사용  
} 
void ultrasonic_trigger()
{
	TRIG_PORT &= ~(1<<TRIG); //LOW
	_delay_ms(1);
	TRIG_PORT |= (1<<TRIG);// 1: HIGH
	_delay_ms(15);
	TRIG_PORT &= ~(1<<TRIG); //LOW
}

void led_by_distance(void)
{
	DDRA = 0xff;
	switch(ultrasonic_distance/58)
	{
		case 2:
		PORTA = 0xff;
		break;
		case 3:
		PORTA = 0b01111111;
		break;
		case 4:
		PORTA = 0b00111111;
		break;
		case 5:
		PORTA = 0b00011111;
		break;
		case 6:
		PORTA = 0b00001111;
		break;
		case 7:
		PORTA = 0b00000111;
		break;
		case 8:
		PORTA = 0b00000011;
		break;
		case 9:
		PORTA = 0b00000001;
		break;
	}
	if (ultrasonic_distance/58 > 9)
	{
		if (uled <= 1000)
		{
			PORTA = 0xff;
		}
		else if (uled <= 2000)
		{
			PORTA = 0x00;
		}
		else if (uled > 2000)
		{
			uled = 0;
		}
	}
}