#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#define R1 PB0
#define Y1 PB1
#define G1 PB2

#define R2 PB3
#define Y2 PB4
#define G2 PB5

#define R3 PD0
#define Y3 PD1
#define G3 PD2

#define R4 PD3
#define Y4 PD4
#define G4 PD5

#define HEARTBEAT_PIN PA0

#define GREEN_TIME  15
#define YELLOW_TIME 3

void all_red(void)
{
	PORTB &= ~((1 << G1) | (1 << Y1) |
	(1 << G2) | (1 << Y2));

	PORTD &= ~((1 << G3) | (1 << Y3) |
	(1 << G4) | (1 << Y4));

	PORTB |= (1 << R1) | (1 << R2);
	PORTD |= (1 << R3) | (1 << R4);
}

void set_green(int path)
{
	all_red();

	switch (path)
	{
		case 0:
		PORTB &= ~(1 << R1);
		PORTB |= (1 << G1);
		break;

		case 1:
		PORTB &= ~(1 << R2);
		PORTB |= (1 << G2);
		break;

		case 2:
		PORTD &= ~(1 << R3);
		PORTD |= (1 << G3);
		break;

		case 3:
		PORTD &= ~(1 << R4);
		PORTD |= (1 << G4);
		break;
	}
}

void set_yellow(int path)
{
	switch (path)
	{
		case 0:
		PORTB &= ~(1 << G1);
		PORTB |= (1 << Y1);
		break;

		case 1:
		PORTB &= ~(1 << G2);
		PORTB |= (1 << Y2);
		break;

		case 2:
		PORTD &= ~(1 << G3);
		PORTD |= (1 << Y3);
		break;

		case 3:
		PORTD &= ~(1 << G4);
		PORTD |= (1 << Y4);
		break;
	}
}

void send_heartbeat(void)
{
	PORTA |= (1 << HEARTBEAT_PIN);
	_delay_ms(5);

	PORTA &= ~(1 << HEARTBEAT_PIN);
	_delay_ms(45);
}

void wait_with_heartbeat(int seconds)
{
	for (int i = 0; i < seconds; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			send_heartbeat();
		}
	}
}

int main(void)
{
	DDRB |= (1 << R1) | (1 << Y1) | (1 << G1) |
	(1 << R2) | (1 << Y2) | (1 << G2);

	DDRD |= (1 << R3) | (1 << Y3) | (1 << G3) |
	(1 << R4) | (1 << Y4) | (1 << G4);

	DDRA |= (1 << HEARTBEAT_PIN);

	PORTA &= ~(1 << HEARTBEAT_PIN);

	all_red();

	while (1)
	{
		for (int path = 0; path < 4; path++)
		{
			set_green(path);

			wait_with_heartbeat(GREEN_TIME);

			set_yellow(path);

			wait_with_heartbeat(YELLOW_TIME);
		}
	}
}

