/*
 * master_avr.c
 * ATmega328P AVR-GCC version of the given Arduino sketch
 *
 * Function kept the same:
 * - Read water sensor 1 from A0
 * - Request water sensor 2 from I2C slave address 8
 * - Read temperature and humidity from AM2302 / DHT22 on D2
 * - Send clean CSV data to ESP32
 *
 * Added:
 * - Show realtime data in Ubuntu screen app every 1 second
 *
 * Hardware:
 *   A0  -> Water sensor 1
 *   D2  -> DHT22 data
 *   A4  -> SDA
 *   A5  -> SCL
 *   D10 -> Soft UART TX to ESP32 RX
 *   D1  -> USB serial TX to Ubuntu screen app
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

/* -----------------------------
   Definitions
----------------------------- */
#define SLAVE_ADDRESS     8
#define DHT_DATA_PIN      PD2
#define DHT_DDR           DDRD
#define DHT_PORT          PORTD
#define DHT_PINREG        PIND

#define SOFT_TX_PIN       PB2
#define SOFT_TX_DDR       DDRB
#define SOFT_TX_PORT      PORTB

/* -----------------------------
   Global variables
----------------------------- */
static int adc1 = 0;
static int adc2 = 0;
static float temperature = 0.0f;
static float humidity = 0.0f;

/* -----------------------------
   UART0 for Ubuntu screen app
----------------------------- */
void uart0_init(unsigned long baud)
{
    uint16_t ubrr = (F_CPU / 16UL / baud) - 1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   // 8N1
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

/* -----------------------------
   Software UART TX for ESP32
   9600 baud on D10 / PB2
----------------------------- */
void soft_uart_init(void)
{
    SOFT_TX_DDR |= (1 << SOFT_TX_PIN);
    SOFT_TX_PORT |= (1 << SOFT_TX_PIN);   // idle high
}

void soft_uart_tx_char(char c)
{
    uint8_t i;

    // start bit
    SOFT_TX_PORT &= ~(1 << SOFT_TX_PIN);
    _delay_us(104);

    // 8 data bits
    for (i = 0; i < 8; i++) {
        if (c & (1 << i))
            SOFT_TX_PORT |= (1 << SOFT_TX_PIN);
        else
            SOFT_TX_PORT &= ~(1 << SOFT_TX_PIN);

        _delay_us(104);
    }

    // stop bit
    SOFT_TX_PORT |= (1 << SOFT_TX_PIN);
    _delay_us(104);
}

void soft_uart_tx_string(const char *s)
{
    while (*s) {
        soft_uart_tx_char(*s++);
    }
}

/* -----------------------------
   ADC
----------------------------- */
void adc_init(void)
{
    ADMUX = (1 << REFS0); // AVcc reference
    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

uint16_t adc_read(uint8_t ch)
{
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

/* -----------------------------
   TWI / I2C Master
----------------------------- */
void twi_init(void)
{
    TWSR = 0x00;
    TWBR = 72;  // ~100kHz at 16MHz
    TWCR = (1 << TWEN);
}

uint8_t twi_start(uint8_t address_rw)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t status = TWSR & 0xF8;
    if (status != 0x08 && status != 0x10)
        return 0;

    TWDR = address_rw;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    status = TWSR & 0xF8;
    if (status != 0x40 && status != 0x18)
        return 0;

    return 1;
}

void twi_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO));
}

uint8_t twi_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

uint8_t twi_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

/* -----------------------------
   DHT22 / AM2302 low-level
----------------------------- */
static inline void dht_pin_output(void)
{
    DHT_DDR |= (1 << DHT_DATA_PIN);
}

static inline void dht_pin_input(void)
{
    DHT_DDR &= ~(1 << DHT_DATA_PIN);
}

static inline void dht_low(void)
{
    DHT_PORT &= ~(1 << DHT_DATA_PIN);
}

static inline void dht_high(void)
{
    DHT_PORT |= (1 << DHT_DATA_PIN);
}

static inline uint8_t dht_read_level(void)
{
    return (DHT_PINREG & (1 << DHT_DATA_PIN)) ? 1 : 0;
}

/*
 * Returns 0 if success
 * Keeps same idea as Arduino code:
 * only update temperature/humidity when reading succeeds
 */
int dht_read(float *temp, float *hum)
{
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint8_t i, j;
    uint16_t timeout;

    // start signal
    dht_pin_output();
    dht_low();
    _delay_ms(2);
    dht_high();
    _delay_us(30);
    dht_pin_input();

    // response sequence
    timeout = 0;
    while (dht_read_level()) {
        _delay_us(1);
        if (++timeout > 120) return -1;
    }

    timeout = 0;
    while (!dht_read_level()) {
        _delay_us(1);
        if (++timeout > 120) return -2;
    }

    timeout = 0;
    while (dht_read_level()) {
        _delay_us(1);
        if (++timeout > 120) return -3;
    }

    // read 40 bits
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {
            timeout = 0;
            while (!dht_read_level()) {
                _delay_us(1);
                if (++timeout > 120) return -4;
            }

            _delay_us(35);

            if (dht_read_level()) {
                data[j] |= (1 << (7 - i));
            }

            timeout = 0;
            while (dht_read_level()) {
                _delay_us(1);
                if (++timeout > 120) return -5;
            }
        }
    }

    // checksum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return -6;
    }

    uint16_t raw_h = ((uint16_t)data[0] << 8) | data[1];
    uint16_t raw_t = ((uint16_t)data[2] << 8) | data[3];

    *hum = raw_h / 10.0f;

    if (raw_t & 0x8000) {
        raw_t &= 0x7FFF;
        *temp = -(raw_t / 10.0f);
    } else {
        *temp = raw_t / 10.0f;
    }

    return 0;
}

/* -----------------------------
   Helper functions
----------------------------- */
void float_to_str_1dp(float value, char *buf)
{
    dtostrf(value, 0, 1, buf);
}

void send_clean_csv_to_esp32(float t, float h, int a1, int a2)
{
    char buf[20];

    float_to_str_1dp(t, buf);
    soft_uart_tx_string(buf);
    soft_uart_tx_char(',');

    float_to_str_1dp(h, buf);
    soft_uart_tx_string(buf);
    soft_uart_tx_char(',');

    itoa(a1, buf, 10);
    soft_uart_tx_string(buf);
    soft_uart_tx_char(',');

    itoa(a2, buf, 10);
    soft_uart_tx_string(buf);
    soft_uart_tx_string("\r\n");
}

void display_screen_data(float t, float h, int a1, int a2)
{
    char buf[20];

    uart0_tx_string("Temperature: ");
    float_to_str_1dp(t, buf);
    uart0_tx_string(buf);

    uart0_tx_string(" C, Humidity: ");
    float_to_str_1dp(h, buf);
    uart0_tx_string(buf);

    uart0_tx_string(" %, ADC1: ");
    itoa(a1, buf, 10);
    uart0_tx_string(buf);

    uart0_tx_string(", ADC2: ");
    itoa(a2, buf, 10);
    uart0_tx_string(buf);

    uart0_tx_string("\r\n");
}

/* -----------------------------
   Main
----------------------------- */
int main(void)
{
    uart0_init(9600);      // Ubuntu screen app
    soft_uart_init();      // ESP32
    adc_init();
    twi_init();

    dht_pin_input();
    dht_high();            // enable pull-up on DHT line

    _delay_ms(3000);

    while (1) {
        // Read Sensor 1
        adc1 = adc_read(0);   // A0

        // Read Sensor 2 from I2C slave
        if (twi_start((SLAVE_ADDRESS << 1) | 1)) {
            uint8_t high = twi_read_ack();
            uint8_t low  = twi_read_nack();
            twi_stop();

            adc2 = ((uint16_t)high << 8) | low;
        }

        // Read temperature and humidity
        if (dht_read(&temperature, &humidity) == 0) {
            // same as Arduino logic:
            // update only on successful read
        }

        // Send only clean data to ESP32
        send_clean_csv_to_esp32(temperature, humidity, adc1, adc2);

        // Display data in Ubuntu screen app now
        display_screen_data(temperature, humidity, adc1, adc2);

        // refresh same current values after 1 second
        _delay_ms(1000);
        display_screen_data(temperature, humidity, adc1, adc2);

        // total loop time kept at 2 seconds
        _delay_ms(1000);
    }
}
