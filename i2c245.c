//
// I2C245
// I2C Master for FT245R
//
// Based on Maxim(Dallas) AN3315
//

#include <ftdi.h>

/*
 * Device handle
 */
struct ftdi_context ftdic;

/*
 * Initialize FT245RL
 */
int i2c245_init_device()
{
    int f;

    ftdi_init(&ftdic);
    f = ftdi_usb_open(&ftdic, 0x0403, 0x6001);

    // return error when cannnot open device.
    if (f < 0 && f != -5)
    {
        return -1;
    }

    // Set bitbang mode(after reset bitbang mode)
    f = ftdi_set_bitmode(&ftdic, 0x00, BITMODE_RESET);
    sleep(1);
    f = ftdi_set_bitmode(&ftdic, 0x00, BITMODE_RESET);

    // return error when cannot set bitbang mode.
    if (f < 0)
    {
        return -1;
    }

    return 1;
}

/*
 * Close connection to FT245RL
 */
int i2c245_close_device()
{
    ftdi_disable_bitbang(&ftdic);
    ftdi_usb_close(&ftdic);
    ftdi_deinit(&ftdic);

    return 1;
}

/*
 * Performs start condition
 */
int i2c245_start()
{
    unsigned char buf;

    ftdi_read_data(&ftdic, buf, sizeof(buf));

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

/*
 * Performs stop condition
 */
int i2c245_stop()
{
    unsigned char buf;

    ftdi_read_data(&ftdic, buf, sizeof(buf));

    // Make SCL low

    // Make SDA low

    // Bring SCL high

    // Bring SDA high
}
