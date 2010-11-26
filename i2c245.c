//
// I2C245
// I2C Master for FT245R
//
// Based on Maxim(Dallas) AN3315
//

#include "i2c245.h"
#include <ftdi.h>
#include <time.h>


/**
 * Initialize FT245RL
 */
int i2c245_init_device(int vendor, int product, int scl, int sda_in, int sda_out)
{
    int f;
    unsigned char bitmask;

    // Set delay to default
    i2c245_set_delay(DELAY_NSEC);

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

    ftdi_read_data(&ftdic, &buf, sizeof(buf));

    // Set SDA high
    set_sda_high();
    delay(0.5);
    // Set SCL high
    set_scl_high();
    delay(0.5);
    // Bring SDA low
    set_sda_low();
    delay(0.5);
    // Bring SCL low
    set_scl_low();
    delay(0.5);

    return 1;
}

/**
 * Performs stop condition
 */
int i2c245_stop()
{
    unsigned char buf;

    ftdi_read_data(&ftdic, &buf, sizeof(buf));

    // Make SCL low
    set_scl_low();
    delay(0.5);
    // Make SDA low
    set_sda_low();
    delay(0.5);
    // Bring SCL high
    set_sda_high();
    delay(0.5);
    // Bring SDA high
    set_scl_high();
    delay(0.5);

    return 1;
}

/**
 * Set delay time
 */
int i2c245_set_delay(int nsec)
{
    tv_nsec.nsec = nsec;

    return 1;
}

/**
 * Set SCL high
 */
static int set_scl_high()
{
    unsigned char buf;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    buf |= 0x01 << pin_assign.scl;
    ftdi_write_data(&ftdic, &buf, sizeof(buf));

    return 1;
}

/**
 * Set SCL low
 */
static int set_scl_low()
{
    unsigned char buf;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    buf &= ~(0x01 << pin_assign.scl);
    ftdi_write_data(&ftdic, &buf, sizeof(buf));

    return 1;
}

/**
 * Set SDA high
 */
static int set_sda_high()
{
    unsigned char buf;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    buf &= ~(0x01 << pin_assign.sda_out);
    ftdi_write_data(&ftdic, &buf, sizeof(buf));

    return 1;
}

/**
 * Set SDA low
 */
static int set_sda_low()
{
    unsigned char buf;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    buf &= ~(0x01 << pin_assign.sda_out);
    ftdi_write_data(&ftdic, &buf, sizeof(buf));

    return 1;
}

/**
 * Get SDA level
 */
static int get_sda()
{
    unsigned char buf;
    int level;

    // TODO: check return value of libftdi's functions
    ftdi_read_data(&ftdic, &buf, sizeof(buf));
    level = (buf >> pin_assign.sda_in) & 1;

    return level;
}

static int delay(double multiple)
{
    struct timespec req;

    req.tv_sec  = 0;
    req.tv_nsec = tv_nsec.nsec * multiple;

    return nanosleep(&req, NULL);
}
