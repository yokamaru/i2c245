//
// I2C245
// I2C Master for FT245R
//
// Based on Maxim(Dallas) AN3315
//

#include <ftdi.h>
#include <time.h>

#include "i2c245.h"

#define _POSIX_C_SOURCE 199309

/**
 * Initialize FT245RL
 */
int i2c245_init_device(int vendor, int product, int scl,
                       int sda_in, int sda_out)
{
    int f;
    unsigned char bitmask;

    // Set delay to default
    i2c245_set_delay(0, DELAY_NSEC);

    ftdi_init(&ftdic);
    f = ftdi_usb_open(&ftdic, vendor, product);

    // return error when cannnot open device.
    if (f < 0 && f != -5)
    {
        return -1;
    }

    // Set pin assign
    // TODO: check given value
    pin_assign.scl = scl;
    pin_assign.sda_in = sda_in;
    pin_assign.sda_out = sda_out;

    // Set bitbang mode(after reset bitbang mode)
    bitmask = (0x01 << pin_assign.scl) | (0x01 << pin_assign.sda_out);
    f = ftdi_set_bitmode(&ftdic, 0xFF, BITMODE_RESET);
    usleep(500);
    f = ftdi_set_bitmode(&ftdic, bitmask, BITMODE_BITBANG);

    // return error when cannot set bitbang mode.
    if (f < 0)
    {
        return -1;
    }

    return 1;
}

/**
 * Close connection to FT245RL
 */
int i2c245_close_device()
{
    ftdi_disable_bitbang(&ftdic);
    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);

    return 1;
}

/**
 * Performs start condition
 */
int i2c245_start()
{
    unsigned char buf;
    int buf_size;

    buf_size = sizeof(buf);
    ftdi_read_data(&ftdic, &buf, buf_size);

    // Set SDA high
    set_sda_high(&buf, buf_size);
    delay(0.5);
    // Set SCL high
    set_scl_high(&buf, buf_size);
    delay(0.5);
    // Set SDA low
    set_sda_low(&buf, buf_size);
    delay(0.5);
    // Set SCL low
    set_scl_low(&buf, buf_size);
    delay(0.5);

    return 1;
}

/**
 * Performs stop condition
 */
int i2c245_stop()
{
    unsigned char buf;
    int buf_size;

    buf_size = sizeof(buf);
    ftdi_read_data(&ftdic, &buf, buf_size);

    // Set SCL low
    set_scl_low(&buf, buf_size);
    delay(0.5);
    // Set SDA low
    set_sda_low(&buf, buf_size);
    delay(0.5);
    // Set SCL high
    set_sda_high(&buf, buf_size);
    delay(0.5);
    // Set SDA high
    set_scl_high(&buf, buf_size);
    delay(0.5);

    return 1;
}

/**
 * Write data
 */
int i2c245_write(unsigned char *data)
{
    unsigned char write_queue;
    unsigned char state_buf;
    int buf_size;
    int i;

    buf_size = sizeof(state_buf);
    ftdi_read_data(&ftdic, &state_buf, buf_size);

    write_queue = *data;

    for (i = 0; i < 8; i++)
    {
        if (write_queue & 0x80)
        {
            // Write '1'

            // Set SDA high
            set_sda_high(&state_buf, buf_size);
            delay(0.5);

            // Supply clock
            set_scl_high(&state_buf, buf_size);
            delay(0.5);
            set_scl_low(&state_buf, buf_size);
            delay(0.5);

            // Set SDA low
            set_sda_low(&state_buf, buf_size);
            delay(0.5);
        }
        else
        {
            // Write '0'

            // Set SDA low
            set_sda_low(&state_buf, buf_size);
            delay(0.5);

            // Supply clock
            set_scl_high(&state_buf, buf_size);
            delay(0.5);
            set_scl_low(&state_buf, buf_size);
            delay(0.5);
        }

        write_queue = write_queue << 1;
    }

    // Release SDA(same as set sda high)
    set_sda_high(&state_buf, buf_size);
    delay(0.5);


    // TODO: check ACK/NACK from slave

    return 0;
}


/**
 * Read data without ack
 */
int i2c245_read_nack(unsigned char *read_data)
{
    unsigned char read_buf;
    unsigned char state_buf;
    int state_buf_size;

    read_buf = 0x00;

    state_buf_size = sizeof(state_buf);
    ftdi_read_data(&ftdic, &state_buf, state_buf_size);

    // Read SDA
    getbyte_sda(&state_buf, state_buf_size, &read_buf);

    //
    // reply nack
    //
    // Set SDA high
    set_sda_high(&state_buf, state_buf_size);
    delay(0.5);

    // Set SCL high
    set_scl_high(&state_buf, state_buf_size);
    delay(0.5);

    // Set SCL low
    set_scl_low(&state_buf, state_buf_size);
    delay(0.5);

    *read_data = read_buf;

    return 0;
}

/**
 * Read data with ack
 */
int i2c245_read_ack(unsigned char *read_data)
{
    unsigned char read_buf;
    unsigned char state_buf;
    int state_buf_size;

    read_buf = 0x00;

    state_buf_size = sizeof(state_buf);
    ftdi_read_data(&ftdic, &state_buf, state_buf_size);

    // Read SDA
    getbyte_sda(&state_buf, state_buf_size, &read_buf);

    //
    // reply ack
    //
    // Set SDA low
    set_sda_low(&state_buf, state_buf_size);
    delay(0.5);

    // Set SCL high
    set_scl_high(&state_buf, state_buf_size);
    delay(0.5);

    // Set SCL low
    set_scl_low(&state_buf, state_buf_size);
    delay(0.5);

    // Set SSA high
    set_sda_high(&state_buf, state_buf_size);
    delay(0.5);

    *read_data = read_buf;

    return 0;
}

/**
 * Set delay time
 */
int i2c245_set_delay(time_t sec, long nsec)
{
    tv_delay.sec = sec;
    tv_delay.nsec = nsec;

    return 1;
}

/**
 * Set SCL high
 */
static int set_scl_high(unsigned char *buf, int size)
{
    // TODO: check return value of libftdi's functions
    *buf &= ~(0x01 << pin_assign.scl);
    ftdi_write_data(&ftdic, buf, size);

    return 1;
}

/**
 * Set SCL low
 */
static int set_scl_low(unsigned char *buf, int size)
{
    // TODO: check return value of libftdi's functions
    *buf |= 0x01 << pin_assign.scl;
    ftdi_write_data(&ftdic, buf, size);

    return 1;
}

/**
 * Set SDA high
 */
static int set_sda_high(unsigned char *buf, int size)
{
    // TODO: check return value of libftdi's functions
    *buf &= ~(0x01 << pin_assign.sda_out);
    ftdi_write_data(&ftdic, buf, size);

    return 1;
}

/**
 * Set SDA low
 */
static int set_sda_low(unsigned char *buf, int size)
{
    // TODO: check return value of libftdi's Functions
    *buf |= 0x01 << pin_assign.sda_out;
    ftdi_write_data(&ftdic, buf, size);

    return 1;
}

/**
 * Get SDA level
 *
 * This function return "sense" level.
 * Signal is inverted on 7405. So '0' means '1', also '1' means '0'.
 */
static int get_sda()
{
    unsigned char buf;
    int level;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    level = ~(buf >> pin_assign.sda_in) & 0x01;

    return level;
}

/**
 * Get SDA levels
 */
static int getbyte_sda(unsigned char *state_buf, int state_buf_size,
                       unsigned char *read_data)
{
    unsigned char read_buf;
    int read_level;
    int i;

    read_buf = 0x00;

    // Set SDA high and SCL low
    set_sda_high(state_buf, state_buf_size);
    set_scl_low(state_buf, state_buf_size);
    delay(0.5);

    for (i = 0; i < 8; i++)
    {
        // Set SCL high
        set_scl_high(state_buf, state_buf_size);
        delay(0.5);

        // Read SDA
        read_level = get_sda();

        // Set SCL low
        set_scl_low(state_buf, state_buf_size);
        delay(0.5);

        read_buf = read_buf << 1;
        if (read_level == 1)
        {
            read_buf |= 0x01;
        }
    }

    *read_data = read_buf;

    return 0;
}

static int delay(double multiple)
{
    struct timespec req;
    long long int time_total;

    req.tv_sec  = tv_delay.sec;
    req.tv_nsec = tv_delay.nsec * multiple;

    // FIXME: overflow time_total
    time_total = (tv_delay.sec * 1e9 + tv_delay.nsec) * multiple;

    req.tv_sec = (long)time_total / 1e9;
    req.tv_nsec = (time_t)(time_total - req.tv_sec * 1e9);

    return nanosleep(&req, NULL);
}
