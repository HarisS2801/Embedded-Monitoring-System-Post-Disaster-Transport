/*
 * slave_avr.c
 * ATmega328P AVR-GCC version of:
 *
 *   Wire.begin(SLAVE_ADDRESS);
 *   Wire.onRequest(sendADC);
 *   adcValue2 = analogRead(A0);
 *
 * Added:
 *   Display realtime ADC value every 1 second on Ubuntu screen app
 *
 * Hardware:
 *   A0 / PC0  -> Water sensor
 *   SDA / PC4 -> I2C SDA
 *   SCL / PC5 -> I2C SCL
 *   TX / PD1  -> USB serial to Ubuntu screen app
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

#define SLAVE_ADDRESS 8
#define WATER_SENSOR_CHANNEL 0   // A0 = ADC0

volatile uint16_t adcValue2 = 0;

/* -----------------------------
   UART0 for Ubuntu screen app
----------------------------- */
void uart0_init(unsigned long baud)
{
    uint16_t ubrr = (F_CPU / 16UL / baud) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);                         // TX enable
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);       // 8 data bits, 1 stop, no parity
}

void uart0_tx_char(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart0_tx_string(const char *s)
{
    while (*s) {
        uart0_tx_char(*s++);
    }
}

void uart0_tx_uint16(uint16_t value)
{
    char buf[8];
    itoa(value, buf, 10);
    uart0_tx_string(buf);
}

/* -----------------------------
   ADC
----------------------------- */
void adc_init(void)
{
    ADMUX = (1 << REFS0); // AVcc reference, ADC0 default
    ADCSRA = (1 << ADEN)  |
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   // prescaler 128
}

uint16_t adc_read(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

/* -----------------------------
   TWI / I2C slave
----------------------------- */
void twi_slave_init(uint8_t address)
{
    TWAR = (address << 1);  // own slave address
    TWCR = (1 << TWEN) |    // enable TWI
           (1 << TWEA) |    // enable ACK
           (1 << TWIE) |    // enable interrupt
           (1 << TWINT);    // clear interrupt flag
}

/*
 * Equivalent to Wire.onRequest(sendADC)
 * Sends:
 *   high byte first
 *   low byte second
 */
ISR(TWI_vect)
{
    static uint8_t send_state = 0;
    uint8_t status = TWSR & 0xF8;

    switch (status) {

        // Own SLA+R received, ACK returned
        case 0xA8:
        // Arbitration lost, own SLA+R received
        case 0xB0:
            TWDR = (uint8_t)(adcValue2 >> 8);   // high byte first
            send_state = 1;
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;

        // Data transmitted, ACK received
        case 0xB8:
            if (send_state == 1) {
                TWDR = (uint8_t)(adcValue2 & 0xFF); // low byte second
                send_state = 2;
            } else {
                TWDR = 0x00;
            }
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;

        // Data transmitted, NACK received
        case 0xC0:
        // Last data transmitted, ACK received
        case 0xC8:
            send_state = 0;
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;

        default:
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE) | (1 << TWINT);
            break;
    }
}

/* -----------------------------
   Display current ADC value
----------------------------- */
void display_adc_to_screen(uint16_t value)
{
    uart0_tx_string("Slave ADC2: ");
    uart0_tx_uint16(value);
    uart0_tx_string("\r\n");
}

/* -----------------------------
   Main
----------------------------- */
int main(void)
{
    uart0_init(9600);          // Ubuntu screen app
    adc_init();
    twi_slave_init(SLAVE_ADDRESS);

    sei();                     // enable global interrupts

    uint16_t screen_counter_ms = 0;

    while (1) {
        // Same algorithm:
        // continuously read the water sensor
        adcValue2 = adc_read(WATER_SENSOR_CHANNEL);

        _delay_ms(50);
        screen_counter_ms += 50;

        // Added functionality:
        // display realtime data every 1 second
        if (screen_counter_ms >= 1000) {
            display_adc_to_screen(adcValue2);
            screen_counter_ms = 0;
        }
    }
}
