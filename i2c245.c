//
// I2C245
// I2C Master for FT245R
//
// Based on Maxim(Dallas) AN3315
//

#include <ftdi.h>

/**
 * Device handle
 */
static struct ftdi_context ftdic;

/**
 * Pin assign
 */
static struct {
    int scl;
    int sda_in;
    int sda_out;
} pin_assign;

/**
 * Initialize FT245RL
 */
int i2c245_init_device(int vendor, int product, int scl, int sda_in, int sda_out)
{
    int f;
    unsigned char bitmask;

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
    buf = buf & 0xFF;

    // Set SCL high
    buf = buf & 0xFF;

    // Bring SDA low
    buf = buf | 0xFF;

    // Bring SCL low
    buf = buf | 0xFF;

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

    // Make SDA low

    // Bring SCL high

    // Bring SDA high
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
