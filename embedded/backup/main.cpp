#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

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


#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

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
#define SELECT_PIN PC0

#define GREEN_TIME_MS  15000UL
#define YELLOW_TIME_MS 3000UL
#define TIMEOUT_MS     150

volatile uint16_t timeout_counter = 0;
volatile uint32_t phase_elapsed = 0;

ISR(TIMER1_COMPA_vect)
{
    static uint8_t previous_state = 0;

    uint8_t current_state =
        (PINA & (1 << HEARTBEAT_PIN)) ? 1 : 0;

    if (current_state && !previous_state)
    {
        timeout_counter = 0;
    }
    else
    {
        if (timeout_counter < TIMEOUT_MS)
            timeout_counter++;
    }

    previous_state = current_state;

    phase_elapsed++;
}

void timer1_init(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11);

    OCR1A = 124;
    TIMSK |= (1 << OCIE1A);
}

uint32_t get_elapsed(void)
{
    uint32_t value;

    uint8_t old_sreg = SREG;
    cli();
    value = phase_elapsed;
    SREG = old_sreg;

    return value;
}

void reset_elapsed(void)
{
    uint8_t old_sreg = SREG;
    cli();
    phase_elapsed = 0;
    SREG = old_sreg;
}

uint16_t get_timeout(void)
{
    uint16_t value;

    uint8_t old_sreg = SREG;
    cli();
    value = timeout_counter;
    SREG = old_sreg;

    return value;
}

void all_red(void)
{
    PORTB &= ~((1 << G1) | (1 << Y1) |
               (1 << G2) | (1 << Y2));

    PORTD &= ~((1 << G3) | (1 << Y3) |
               (1 << G4) | (1 << Y4));

    PORTB |= (1 << R1) | (1 << R2);
    PORTD |= (1 << R3) | (1 << R4);
}

void set_green(uint8_t path)
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

void set_yellow(uint8_t path)
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

int main(void)
{
    DDRB |= (1 << R1) | (1 << Y1) | (1 << G1) |
            (1 << R2) | (1 << Y2) | (1 << G2);

    DDRD |= (1 << R3) | (1 << Y3) | (1 << G3) |
            (1 << R4) | (1 << Y4) | (1 << G4);

    DDRA &= ~(1 << HEARTBEAT_PIN);
    PORTA &= ~(1 << HEARTBEAT_PIN);

    DDRC |= (1 << SELECT_PIN);
    PORTC &= ~(1 << SELECT_PIN);

    all_red();

    timer1_init();
    sei();

    uint8_t path = 0;
    uint8_t phase = 0;  // 0 = Green, 1 = Yellow

    uint32_t phase_duration = GREEN_TIME_MS;

    /*
       Backup starts its own traffic cycle immediately.
       Its 74HC244 outputs remain disabled while Master is selected.
    */

    set_green(path);

    while (1)
    {
        uint32_t elapsed = get_elapsed();

        /* Transfer control after heartbeat timeout */
        if (get_timeout() >= TIMEOUT_MS)
        {
            PORTC |= (1 << SELECT_PIN);
        }

        /* Change traffic light phase */
        if (elapsed >= phase_duration)
        {
            reset_elapsed();

            if (phase == 0)
            {
                set_yellow(path);

                phase = 1;
                phase_duration = YELLOW_TIME_MS;
            }
            else
            {
                path = (path + 1) % 4;

                set_green(path);

                phase = 0;
                phase_duration = GREEN_TIME_MS;
            }
        }
    }
}
